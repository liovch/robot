#include "imageprovider.h"
#include <QDir>
#include <QDebug>

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

    if (!image.isNull())
        emit nextImage(image);
}
