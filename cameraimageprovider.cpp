#include "cameraimageprovider.h"
#include <QDebug>
#include "camera.h"

CameraImageProvider::CameraImageProvider(QObject *parent) :
    ImageProvider(parent),
    m_camera(0),
    m_imageCapture(0)
{
}

bool CameraImageProvider::init()
{
    m_camera = new QCamera(this);
    m_imageCapture = new QCameraImageCapture(m_camera, this);
    if (!m_camera->isCaptureModeSupported(QCamera::CaptureStillImage)) {
        qDebug() << "Still image capture mode is not supported";
        return false;
    }

    // TODO: Connect to camera error signal
    // connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));

    m_camera->setCaptureMode(QCamera::CaptureStillImage);
    //connect(m_imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(m_imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    //connect(m_imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    return true;
}

void CameraImageProvider::requestNextImage()
{
    Q_ASSERT(m_imageCapture);
    m_imageCapture->capture();
}

void CameraImageProvider::processCapturedImage(int id, const QImage &preview)
{
    Q_UNUSED(id);

    QImage image = preview;
    if (gCamera.scale()) {
        int newHeight = image.height() / (2 << gCamera.scale());
        image = image.scaledToHeight(newHeight, Qt::SmoothTransformation);
    }

    if (!image.isNull())
        emit nextImage(image);
}
