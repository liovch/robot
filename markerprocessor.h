#ifndef MARKERPROCESSOR_H
#define MARKERPROCESSOR_H

#include <QObject>
#include "marker.h"

class MarkerProcessor : public QObject
{
    Q_OBJECT
public:
    explicit MarkerProcessor(QObject *parent = 0);

signals:
    
public slots:
    void processMarkers(const QList<Marker>& markers);

private:
    QList<Marker> m_markers;
};

#endif // MARKERPROCESSOR_H
