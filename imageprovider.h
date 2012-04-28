#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QFileInfo>
#include <QImage>

class ImageProvider : public QObject
{
    Q_OBJECT
public:
    explicit ImageProvider(QObject *parent = 0);

    void setDir(const QString& path, const QString& nameFilter);

signals:
    void nextImage(const QImage& image);

public slots:
    void requestNextImage();

private:
    QFileInfoList m_files; // TODO: For now just use predefined images from a folder
};

#endif // IMAGEPROVIDER_H
