#include "imageprovider.h"
#include <QDir>
#include <QDebug>
#include "camera.h"

ImageProvider::ImageProvider(QObject *parent) :
    QObject(parent)
{
}

void ImageProvider::setDir(const QString &path, const QString &nameFilter)
{
    m_files = QDir(path, nameFilter).entryInfoList();
}

void ImageProvider::requestNextImage()
{
    QFileInfo file;
    QImage image;
    do {
        if (m_files.empty())
            return;
        file = m_files.takeFirst();
        qDebug() << "Loading file:" << file.absoluteFilePath();
    } while (!file.exists() || !image.load(file.absoluteFilePath()));

    if (gCamera.scale()) {
        int newHeight = image.height() / (2 << gCamera.scale());
        image = image.scaledToHeight(newHeight, Qt::SmoothTransformation);
    }

    if (!image.isNull())
        emit nextImage(image);
}
