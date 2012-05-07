#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QImage>

class ImageProvider : public QObject
{
    Q_OBJECT
public:
    explicit ImageProvider(QObject *parent = 0);
    virtual ~ImageProvider() { }
    
signals:
    void nextImage(const QImage& image);

public slots:
    virtual void requestNextImage() = 0;
};

#endif // IMAGEPROVIDER_H
