#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <QObject>
#include "marker.h"

class SensorData : public QObject
{
    Q_OBJECT
public:
    explicit SensorData(QObject *parent = 0);
    SensorData(const SensorData& data);
    
    QList<Marker> m_markers;
    qreal m_angle;

    SensorData& operator =(const SensorData& data);
};

#endif // SENSORDATA_H
