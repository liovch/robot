#include "sensordata.h"

SensorData::SensorData(QObject *parent) :
    QObject(parent),
    m_angle(0.0)
{
}

SensorData::SensorData(const SensorData &data) :
    QObject(data.parent())
{
    operator =(data);
}

SensorData &SensorData::operator =(const SensorData &data)
{
    m_markers = data.m_markers;
    m_angle = data.m_angle;
    return *this;
}
