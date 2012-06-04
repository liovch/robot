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

ARToolkitImageProcessor::ARToolkitImageProcessor(QObject *parent) :
    ImageProcessor(parent),
    m_tracker(0),
    m_previousX(0.f),
    m_previousY(0.f),
    m_previousZ(0.f)
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
    if(!m_tracker->init("/home/lev/code/robot/data/artoolkit/N9_3mpix.cal", 1.0f, 1000.0f)) {
#else
    if(!m_tracker->init("/home/developer/artoolkit/N9_3mpix.cal", 1.0f, 1000.0f)) {
#endif
        qDebug() << "Failed to initialize tracker";
        delete m_tracker;
        m_tracker = 0;
        return false;
    }

    // define size of the marker (in mm)
    m_tracker->setPatternWidth(150);

    // the marker in the BCH test image has a thin border...
    m_tracker->setBorderWidth(useBCH ? 0.125f : 0.250f);

    // set a threshold. alternatively we could also activate automatic thresholding
    //m_tracker->setThreshold(150);
    m_tracker->activateAutoThreshold(true);

    // let's use lookup-table undistortion for high-speed
    // note: LUT only works with images up to 1024x1024
    m_tracker->setUndistortionMode(ARToolKitPlus::UNDIST_STD);

    // RPP is more robust than ARToolKit's standard pose estimator
    //m_tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);

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

    // TODO: Find a way to avoid doing fuzzy compare here.
    //       Probably the problem is with pose estimator and
    //       looks like it will be disabled for multimarker mode anyway.
    // here we go, just one call to find the camera pose
    int numMarkers = 0;
    int markerId = m_tracker->calc(image.bits(), -1, true, NULL, &numMarkers);
    float markerX = m_tracker->getModelViewMatrix()[12];
    float markerY = m_tracker->getModelViewMatrix()[13];
    float markerZ = m_tracker->getModelViewMatrix()[14];
    if (markerId >= 0 && (!qFuzzyCompare(markerX, m_previousX) ||
                          !qFuzzyCompare(markerY, m_previousY) ||
                          !qFuzzyCompare(markerZ, m_previousZ))) {
        qDebug() << "Number of markers found:" << numMarkers;
        float conf = (float)m_tracker->getConfidence();
        qDebug() << "Found marker" << markerId << "confidence" << conf * 100.0 << "\nCoordinates:" << markerX << markerY << markerZ;
        m_previousX = markerX;
        m_previousY = markerY;
        m_previousZ = markerZ;
        // Note: There's also m_tracker->getProjectionMatrix()
    } else {
        qDebug() << "No markers found";
    }
}

void ARToolkitLogger::artLog(const char *nStr)
{
    qDebug() << nStr;
}
