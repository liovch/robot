#include "cameraimageprovider.h"
#include <QDebug>
#include "camera.h"

CameraImageProvider::CameraImageProvider(QObject *parent) :
    ImageProvider(parent),
    m_camera(0),
    m_mediaRecorder(0),
    m_imageCapture(0)
{
}

CameraImageProvider::~CameraImageProvider()
{
    delete m_mediaRecorder;
    delete m_imageCapture;
    delete m_camera;
}

bool CameraImageProvider::init()
{
    m_camera = new QCamera();
    m_mediaRecorder = new QMediaRecorder(m_camera);
    m_imageCapture = new QCameraImageCapture(m_camera);
    if (!m_camera->isCaptureModeSupported(QCamera::CaptureStillImage)) {
        qDebug() << "Still image capture mode is not supported";
        return false;
    }

    QImageEncoderSettings imageSettings;
    imageSettings.setCodec("image/jpeg");
    imageSettings.setResolution(2304, 1296);
    m_imageCapture->setEncodingSettings(imageSettings);

    QCameraFocus* focus = m_camera->focus();
    if (!focus || !focus->isAvailable()) {
        qDebug() << "Camera focus mode is not supported.";
        return false;
    }
    // Note: QCameraFocus::HyperfocalFocus and QCameraFocus::FocusPointCenter are not supported
    //focus->setFocusPointMode(QCameraFocus::FocusPointCenter);
    //focus->setFocusMode(QCameraFocus::ManualFocus);

    // TODO: Connect to camera error signal
    // connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));

    m_camera->setCaptureMode(QCamera::CaptureStillImage);
    m_camera->start();

    // Note: imageCapture signal is emited with a low-res version of an image (853 , 480)
    //connect(m_imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    //connect(m_imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(m_imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    return true;
}

void CameraImageProvider::requestNextImage()
{
    Q_ASSERT(m_imageCapture);
    m_imageCapture->capture();
}

void CameraImageProvider::imageSaved(int, const QString &fileName)
{
    QImage image(fileName);

    if (!image.isNull())
        emit nextImage(image);
}
