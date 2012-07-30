#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QList>
#include "marker.h"

class ImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessor(QObject *parent = 0);
    virtual ~ImageProcessor() { }
    
signals:
    // emitted after image processor finishes the current image
    // empty list is emitted if no markers were found on the current image
    void imageProcessed(const QList<Marker>& markers);

public slots:
    // finds markers on an image
    virtual void processImage(const QImage& image) = 0;
};

#endif // IMAGEPROCESSOR_H
