#include "artoolkitimageprocessor.h"
#include "qplatformdefs.h"
#include "ARToolKitPlus/TrackerSingleMarkerImpl.h"
#include <QDebug>

class ARToolkitLogger : public ARToolKitPlus::Logger
{
    void artLog(const char* nStr);
} gARToolkitLogger;

ARToolKitPlus::PIXEL_FORMAT convertPixelFormat(QImage::Format format)
{
    switch (format) {
        case QImage::Format_RGB32: return ARToolKitPlus::PIXEL_FORMAT_RGBA;
        case QImage::Format_RGB888: return ARToolKitPlus::PIXEL_FORMAT_RGB;
        default: qDebug() << "Unsupported image format:" << format; break;
    }

    return (ARToolKitPlus::PIXEL_FORMAT)0;
}

void convertMarkerInfo(Marker& markerInfo, const ARToolKitPlus::ARMarkerInfo& info)
{
    markerInfo.id = info.id;
    markerInfo.confidence = info.cf;
    markerInfo.pos[0] = info.pos[0]; markerInfo.pos[1] = info.pos[1];
    markerInfo.line[0][0] = info.line[0][0]; markerInfo.line[1][0] = info.line[1][0]; markerInfo.line[2][0] = info.line[2][0]; markerInfo.line[3][0] = info.line[3][0];
    markerInfo.line[0][1] = info.line[0][1]; markerInfo.line[1][1] = info.line[1][1]; markerInfo.line[2][1] = info.line[2][1]; markerInfo.line[3][1] = info.line[3][1];
    markerInfo.line[0][2] = info.line[0][2]; markerInfo.line[1][2] = info.line[1][2]; markerInfo.line[2][2] = info.line[2][2]; markerInfo.line[3][2] = info.line[3][2];
    markerInfo.vertex[0][0] = info.vertex[0][0]; markerInfo.vertex[1][0] = info.vertex[1][0]; markerInfo.vertex[2][0] = info.vertex[2][0]; markerInfo.vertex[3][0] = info.vertex[3][0];
    markerInfo.vertex[0][1] = info.vertex[0][1]; markerInfo.vertex[1][1] = info.vertex[1][1]; markerInfo.vertex[2][1] = info.vertex[2][1]; markerInfo.vertex[3][1] = info.vertex[3][1];
}

ARToolkitImageProcessor::ARToolkitImageProcessor(QObject *parent) :
    ImageProcessor(parent),
    m_tracker(0)
{
}

ARToolkitImageProcessor::~ARToolkitImageProcessor()
{
    if (m_tracker) {
        m_tracker->cleanup();
        delete m_tracker;
    }
}

bool ARToolkitImageProcessor::initialize(const QImage& image)
{
    // TODO: We probably should initialize this tracker in the constructor
    bool useBCH = false;

    // create a tracker that does:
    //  - 6x6 sized marker images
    //  - samples at a maximum of 6x6
    //  - works with luminance (gray) images
    //  - can load a maximum of 1 pattern
    //  - can detect a maximum of 8 patterns in one image
    m_tracker = new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 1, 8>(image.width(), image.height());

    // set a logger so we can output error messages
    m_tracker->setLogger(&gARToolkitLogger);

    const char* description = m_tracker->getDescription();
    qDebug() << "ARToolKitPlus compile-time information:" << description;
    m_tracker->setPixelFormat(convertPixelFormat(image.format()));
    //m_tracker->setLoadUndistLUT(true);

    // load a camera file. two types of camera files are supported:
    //  - Std. ARToolKit
    //  - MATLAB Camera Calibration Toolbox
#ifndef MEEGO_EDITION_HARMATTAN
    if(!m_tracker->init("../robot/data/artoolkit/N9_3mpix.cal", 1.0f, 1000.0f)) {
#else
    if(!m_tracker->init("/home/developer/artoolkit/N9_3mpix.cal", 1.0f, 1000.0f)) {
#endif
        qDebug() << "Failed to initialize tracker";
        delete m_tracker;
        m_tracker = 0;
        return false;
    }

    // define size of the marker (in mm)
    m_tracker->setPatternWidth(MarkerParams::size());

    // the marker in the BCH test image has a thin border...
    m_tracker->setBorderWidth(useBCH ? 0.125f : 0.250f); // TODO: Check if border size is correct

    // set a threshold. alternatively we could also activate automatic thresholding
    //m_tracker->setThreshold(150);
    m_tracker->activateAutoThreshold(true);

    // let's use lookup-table undistortion for high-speed
    // note: LUT only works with images up to 1024x1024
    m_tracker->setUndistortionMode(ARToolKitPlus::UNDIST_STD);

    // Pose estimator is required to produce OpenGL matrix
    m_tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_ORIGINAL);

    // switch to simple ID based markers
    // use the tool in tools/IdPatGen to generate markers
    m_tracker->setMarkerMode(useBCH ? ARToolKitPlus::MARKER_ID_BCH : ARToolKitPlus::MARKER_ID_SIMPLE);

    return true;
}

static inline bool qMyFuzzyCompare(float p1, float p2)
{
    return (qAbs(p1 - p2) <= 0.0001f);
}

void ARToolkitImageProcessor::processImage(const QImage &img)
{
    // TODO: Get rid of pixel format conversion
    QImage image = img.convertToFormat(QImage::Format_RGB888);
    //qDebug() << "Image format: " << image.format() << image.width() << image.height() << image.bytesPerLine();

    if (!m_tracker && !initialize(image))
        return;

    // here we go, just one call to find the camera pose
    int numMarkers = 0;
    ARToolKitPlus::ARMarkerInfo *markerInfo = 0;
    int bestMarkerId = m_tracker->calc(image.bits(), -1, false, &markerInfo, &numMarkers);
    if (bestMarkerId < 0) {
        qDebug() << "No markers found";
        return;
    }

    QList<Marker> markers;

    // TODO: Find out why we're getting duplicate values sometimes
    bool isValidMarker = false;
    for (int i = 0; i < numMarkers; i++) {
        if (markerInfo[i].id < 0 && markerInfo[i].cf <= 0.0)
            continue;

        Marker m;
        convertMarkerInfo(m, markerInfo[i]);
        if (m_previousMarkers.contains(m))
            continue;

        qDebug() << "Marker:" << markerInfo[i].id << "Confidence:" << markerInfo[i].cf;
//                 << "pos" << markerInfo[i].pos[0] << markerInfo[i].pos[1] << "\n" <<
//                    "line\n" << markerInfo[i].line[0][0] << markerInfo[i].line[1][0] << markerInfo[i].line[2][0] << markerInfo[i].line[3][0] << "\n" <<
//                              markerInfo[i].line[0][1] << markerInfo[i].line[1][1] << markerInfo[i].line[2][1] << markerInfo[i].line[3][1] << "\n" <<
//                              markerInfo[i].line[0][2] << markerInfo[i].line[1][2] << markerInfo[i].line[2][2] << markerInfo[i].line[3][2] << "\n" <<
//                    "vertex\n" << markerInfo[i].vertex[0][0] << markerInfo[i].vertex[1][0] << markerInfo[i].vertex[2][0] << markerInfo[i].vertex[3][0] << "\n" <<
//                                markerInfo[i].vertex[0][1] << markerInfo[i].vertex[1][1] << markerInfo[i].vertex[2][1] << markerInfo[i].vertex[3][1];

        // TODO: Maybe move this stuff into Marker itself?
        ARFloat nPatternCenter[2];
        nPatternCenter[0] = 0.f; nPatternCenter[1] = 0.f;
        // TODO: Process errors?
        m_tracker->calcOpenGLMatrixFromMarker(&markerInfo[i], nPatternCenter, MarkerParams::size(), m.modelView);

        isValidMarker = true;
        qDebug() << "Coordinates:" << m.modelView[12] << m.modelView[13] << m.modelView[14];
        markers.append(m);
    }

    if (!markers.isEmpty())
        m_previousMarkers = markers;

    if (!isValidMarker)
        qDebug() << "No markers found: reported duplicate markers";

    emit imageProcessed(markers);
}

void ARToolkitLogger::artLog(const char *nStr)
{
    qDebug() << nStr;
}
