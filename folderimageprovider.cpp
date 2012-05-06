#include "folderimageprovider.h"
#include <QDir>
#include <QDebug>
#include <QImageReader>
#include "camera.h"

FolderImageProvider::FolderImageProvider(QObject *parent) :
    ImageProvider(parent)
{
}

void FolderImageProvider::setDir(const QString &path, const QString &nameFilter)
{
    m_files = QDir(path, nameFilter).entryInfoList();
}

void FolderImageProvider::requestNextImage()
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
