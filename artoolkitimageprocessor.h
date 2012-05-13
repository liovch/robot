#ifndef ARTOOLKITIMAGEPROCESSOR_H
#define ARTOOLKITIMAGEPROCESSOR_H

#include "imageprocessor.h"

class ARToolkitImageProcessor : public ImageProcessor
{
    Q_OBJECT
public:
    explicit ARToolkitImageProcessor(QObject *parent = 0);

    // slots:
    void processImage(const QImage& image);

signals:
    
public slots:
    
};

#endif // ARTOOLKITIMAGEPROCESSOR_H
