#include "artoolkitimageprocessor.h"
#include <QScopedPointer>
#include <QDebug>
#include "ARToolKitPlus/TrackerSingleMarkerImpl.h"

ARToolKitPlus::PIXEL_FORMAT convertPixelFormat(QImage::Format format)
{
    switch (format) {
        case QImage::Format_ARGB32: return ARToolKitPlus::PIXEL_FORMAT_ARGB;
        case QImage::Format_RGB32: return ARToolKitPlus::PIXEL_FORMAT_RGB;
        default: qDebug() << "Unsupported image format:" << format; break;
    }

    return (ARToolKitPlus::PIXEL_FORMAT)0;
}

ARToolkitImageProcessor::ARToolkitImageProcessor(QObject *parent) :
    ImageProcessor(parent)
{

}

void ARToolkitImageProcessor::processImage(const QImage &image)
{
    // TODO: We probably should initialize this tracker in the constructor
    bool useBCH = false;

    // create a tracker that does:
    //  - 6x6 sized marker images
    //  - samples at a maximum of 6x6
    //  - works with luminance (gray) images
    //  - can load a maximum of 1 pattern
    //  - can detect a maximum of 8 patterns in one image
    QScopedPointer<ARToolKitPlus::TrackerSingleMarker> tracker(new ARToolKitPlus::TrackerSingleMarkerImpl<6,6,6, 1, 8>(image.width(), image.height()));

    // TODO:
    // const char* description = tracker->getDescription();
    // printf("ARToolKitPlus compile-time information:\n%s\n\n", description);
    // set a logger so we can output error messages
    // tracker->setLogger(&logger);
    tracker->setPixelFormat(convertPixelFormat(image.format()));
    //tracker->setLoadUndistLUT(true);

    // load a camera file. two types of camera files are supported:
    //  - Std. ARToolKit
    //  - MATLAB Camera Calibration Toolbox
    if(!tracker->init("data/artoolkit/N9_3mpix.cal", 1.0f, 1000.0f)) {
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
    for(int i=0; i<16; i++)
        printf("%.2f  %s", tracker->getModelViewMatrix()[i], (i%4==3)?"\n  " : "");
    // Note: There's also tracker->getProjectionMatrix()
}
