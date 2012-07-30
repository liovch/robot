#ifndef ARTOOLKITIMAGEPROCESSOR_H
#define ARTOOLKITIMAGEPROCESSOR_H

#include "imageprocessor.h"

namespace ARToolKitPlus {
    class TrackerSingleMarker;
}

class ARToolkitImageProcessor : public ImageProcessor
{
    Q_OBJECT
public:
    explicit ARToolkitImageProcessor(QObject *parent = 0);
    ~ARToolkitImageProcessor();

    // slots:
    void processImage(const QImage& image);

signals:
    
public slots:
    
private:
    bool initialize(const QImage& image); // called when the first image is received

    // TODO: For some reason cannot use QScopedPointer with forward declared class
    ARToolKitPlus::TrackerSingleMarker *m_tracker;
    QList<Marker> m_previousMarkers;
};

#endif // ARTOOLKITIMAGEPROCESSOR_H
