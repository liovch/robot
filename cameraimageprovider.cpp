#include "cameraimageprovider.h"
#include <QDebug>
#include "camera.h"
#include <QFileInfo>

CameraImageProvider::CameraImageProvider(QObject *parent) :
    ImageProvider(parent),
    m_camera(0)
{
}

void CameraImageProvider::init(QObject *camera)
{
    m_camera = camera;
    QObject::connect(camera, SIGNAL(imageSaved(QString)), this, SLOT(processImageSaved(QString)));
}

void CameraImageProvider::requestNextImage()
{
    Q_ASSERT(m_camera);
    QMetaObject::invokeMethod(m_camera, "captureImage", Qt::AutoConnection);
}

void CameraImageProvider::processImageSaved(const QString &file)
{
    QFileInfo fileInfo(file);
    QImage image(fileInfo.absoluteFilePath());
    if (image.width() != 2304 || image.height() != 1296) {
        qDebug() << "Invalid image resolution";
        return;
    }

    if (gCamera.scale()) {
        int newHeight = image.height() / (2 << gCamera.scale());
        image = image.scaledToHeight(newHeight, Qt::SmoothTransformation);
    }

    if (!image.isNull())
        emit nextImage(image);
}
