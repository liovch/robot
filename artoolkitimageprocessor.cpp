#include "artoolkitimageprocessor.h"
#include "qplatformdefs.h"
#include <QScopedPointer>
#include <QDebug>
#include "ARToolKitPlus/TrackerSingleMarkerImpl.h"

class MyLogger : public ARToolKitPlus::Logger
{
    void artLog(const char* nStr)
    {
        qDebug() << nStr;
    }
};


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
    ImageProcessor(parent)
{

}

void ARToolkitImageProcessor::processImage(const QImage &img)
{
    // TODO: Get rid of pixel format conversion
    QImage image = img.convertToFormat(QImage::Format_RGB888);
    qDebug() << "Image format: " << image.format() << image.width() << image.height() << image.bytesPerLine();

    // TODO: We probably should initialize this tracker in the constructor
    bool useBCH = false;

    // create a tracker that does:
    //  - 6x6 sized marker images
    //  - samples at a maximum of 6x6
    //  - works with luminance (gray) images
    //  - can load a maximum of 1 pattern
    //  - can detect a maximum of 8 patterns in one image
    QScopedPointer<ARToolKitPlus::TrackerSingleMarker> tracker(new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 1, 8>(image.width(), image.height()));

    // set a logger so we can output error messages
    MyLogger logger;
    tracker->setLogger(&logger);

    const char* description = tracker->getDescription();
    qDebug() << "ARToolKitPlus compile-time information:" << description;
    tracker->setPixelFormat(convertPixelFormat(image.format()));
    //tracker->setLoadUndistLUT(true);

    // load a camera file. two types of camera files are supported:
    //  - Std. ARToolKit
    //  - MATLAB Camera Calibration Toolbox
#ifndef MEEGO_EDITION_HARMATTAN
    if(!tracker->init("/home/lev/code/robot/data/artoolkit/N9_3mpix.cal", 1.0f, 1000.0f)) {
#else
    if(!tracker->init("/home/developer/artoolkit/N9_3mpix.cal", 1.0f, 1000.0f)) {
#endif
        qDebug() << "Failed to initialize tracker";
        return;
    }

    // define size of the marker (in mm)
    tracker->setPatternWidth(150);

    // the marker in the BCH test image has a thin border...
    tracker->setBorderWidth(useBCH ? 0.125f : 0.250f);

    // set a threshold. alternatively we could also activate automatic thresholding
    //tracker->setThreshold(150);
    tracker->activateAutoThreshold(true);

    // let's use lookup-table undistortion for high-speed
    // note: LUT only works with images up to 1024x1024
    tracker->setUndistortionMode(ARToolKitPlus::UNDIST_STD);

    // RPP is more robust than ARToolKit's standard pose estimator
    //tracker->setPoseEstimator(ARToolKitPlus::POSE_ESTIMATOR_RPP);

    // switch to simple ID based markers
    // use the tool in tools/IdPatGen to generate markers
    tracker->setMarkerMode(useBCH ? ARToolKitPlus::MARKER_ID_BCH : ARToolKitPlus::MARKER_ID_SIMPLE);

    // here we go, just one call to find the camera pose
    int markerId = tracker->calc(image.bits());
    float conf = (float)tracker->getConfidence();

    qDebug() << "Found marker" << markerId << "confidence" <<  conf * 100.0 << "\nPose-Matrix:\n";
    qDebug() << tracker->getModelViewMatrix()[12]
             << tracker->getModelViewMatrix()[13]
             << tracker->getModelViewMatrix()[14];
    // Note: There's also tracker->getProjectionMatrix()
}
