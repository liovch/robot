#ifndef MARKERPROCESSOR_H
#define MARKERPROCESSOR_H

#include <QObject>
#include "marker.h"

class MarkerProcessor : public QObject
{
    Q_OBJECT
public:
    explicit MarkerProcessor(QObject *parent = 0);

    void setOutputDirectory(const QString& dir) { m_dir = dir; }
    void setImageDisplay(QObject *display) { m_display = display; }

signals:
    
public slots:
    void processMarkers(const QList<Marker>& markers);
    void processMarkerMap(const QVector<MarkerParams::MarkerId>& markerMap, int width, int height);

private:
    QRgb markerColor(MarkerParams::MarkerId id);

    QString m_dir;
    QObject *m_display;
    int m_fileIndex;
    QList<Marker> m_markers;
};

#endif // MARKERPROCESSOR_H
