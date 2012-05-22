#include "declarativecameraimageprovider.h"
#include <QDebug>
#include "camera.h"
#include <QFileInfo>

DeclarativeCameraImageProvider::DeclarativeCameraImageProvider(QObject *parent) :
    ImageProvider(parent),
    m_camera(0)
{
}

void DeclarativeCameraImageProvider::init(QObject *camera)
{
    m_camera = camera;
    QObject::connect(camera, SIGNAL(imageSaved(QString)), this, SLOT(processImageSaved(QString)));
}

void DeclarativeCameraImageProvider::requestNextImage()
{
    Q_ASSERT(m_camera);
    QMetaObject::invokeMethod(m_camera, "captureImage", Qt::AutoConnection);
}

void DeclarativeCameraImageProvider::processImageSaved(const QString &file)
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
