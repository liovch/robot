#ifndef THRESHOLDIMAGEPROCESSOR_H
#define THRESHOLDIMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QList>
#include "marker.h"

class ThresholdImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ThresholdImageProcessor(QObject *parent = 0);

signals:
    // emited when image processor is ready to process the next image
    void needNextImage();
    // emitted after image processor finishes the current image
    // empty list is emitted if no markers were found on the current image
    void newMarkers(const QList<Marker>& markers);
    // emits marker map image with marker locations and middle line
    void newMarkerMap(const QVector<MarkerParams::MarkerId>& markerMap, int width, int height);

public slots:
    // finds markers on an image
    void processImage(const QImage& image);
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
