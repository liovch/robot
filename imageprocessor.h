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

    // finds markers on an image
    QList<Marker> processImage(const QImage& image);

signals:

public slots:

private:
    // apply color threshold for each marker
    QVector<MarkerParams::MarkerId> buildMarkerMap(const QImage& image);
    QList<Marker> buildMarkerList(QVector<MarkerParams::MarkerId> markerMap, int width, int height);
    double computeMarkerHeight(int *markersOnLine, int markerWidth, int lineSize, int &above, int &below);
    void leastSquaresFitting(int *markersOnLine, int width, int height);
};

#endif // IMAGEPROCESSOR_H
