#include "sensordata.h"

SensorData::SensorData(QObject *parent) :
    QObject(parent),
    m_azimuth(0.0)
{
}

SensorData::SensorData(const SensorData &data)
{
    operator =(data);
}

SensorData &SensorData::operator =(const SensorData &data)
{
    m_markers = data.m_markers;
    m_azimuth = data.m_azimuth;
}
