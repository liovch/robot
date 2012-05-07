#include "cameraimageprovider.h"
#include <QDebug>
#include "camera.h"
#include <QDeclarativeImageProvider>

CameraImageProvider::CameraImageProvider(QObject *parent) :
    ImageProvider(parent),
    m_camera(0),
    m_engine(0)
{
}

void CameraImageProvider::init(QObject *camera, QDeclarativeEngine *engine)
{
    m_camera = camera;
    m_engine = engine;
    QObject::connect(camera, SIGNAL(imageCaptured(QString)), this, SLOT(processCapturedImage(QString)));
}

void CameraImageProvider::requestNextImage()
{
    Q_ASSERT(m_camera);
    QMetaObject::invokeMethod(m_camera, "captureImage", Qt::AutoConnection);
}

void CameraImageProvider::processCapturedImage(const QString &preview)
{
    Q_ASSERT(m_engine);
    // TODO: Make sure image provider id is always 'camera'.
    //       Check the preview URL.
    QDeclarativeImageProvider *imageProvider = m_engine->imageProvider("camera");
    if (!imageProvider) {
        qDebug() << "Image provider not found";
        return;
    }

    qDebug() << Q_FUNC_INFO << preview;
    int i = preview.lastIndexOf("/");
    if (i < 0) {
        qDebug() << "Invalid image URL";
        return;
    }

    QString imageID = preview.right(preview.size() - i - 1);
    qDebug() << "Image ID: " << imageID;

    QSize size;
    QImage image = imageProvider->requestImage(imageID, 0, size);

    if (gCamera.scale()) {
        int newHeight = image.height() / (2 << gCamera.scale());
        image = image.scaledToHeight(newHeight, Qt::SmoothTransformation);
    }

    if (!image.isNull())
        emit nextImage(image);
}
