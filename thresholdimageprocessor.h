#ifndef THRESHOLDIMAGEPROCESSOR_H
#define THRESHOLDIMAGEPROCESSOR_H

#include "imageprocessor.h"

class ThresholdImageProcessor : public ImageProcessor
{
    Q_OBJECT
public:
    explicit ThresholdImageProcessor(QObject *parent = 0);

    // slots:
    void processImage(const QImage& image);

public slots:
    void processLastImage();

private:
    // apply color threshold for each marker
    QVector<MarkerParams::MarkerId> buildMarkerMap(const QImage& image);
    QList<Marker> buildMarkerList(QVector<MarkerParams::MarkerId> markerMap, int width, int height);
    qreal computeMarkerHeight(int *markersOnLine, int markerWidth, int lineSize, int &above, int &below);
    void leastSquaresFitting(int *markersOnLine, int width, int height);

    int maxHorizontalGap() const;
    int maxVerticalGap() const;

    QImage m_lastImage;
};

#endif // THRESHOLDIMAGEPROCESSOR_H
