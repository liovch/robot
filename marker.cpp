#include "marker.h"

// TODO: move these to a new camera class
#define PIXEL_HEIGHT   1.478E-6 // physical size of a pixel on the camera sensor (in m)
#define FOCAL_DISTANCE 3.77E-3  // focal distance of the camera (in m)

Marker::Marker(QObject *parent) :
    QObject(parent),
    m_offset(0.0),
    m_width(0.0),
    m_height(0.0),
    m_id(MARKER_NULL),
    m_above(0.0),
    m_below(0.0)
{
}

Marker::Marker(const Marker &marker):
    QObject(marker.parent()),
    m_offset(marker.offset()),
    m_width(marker.width()),
    m_height(marker.height()),
    m_id(marker.id()),
    m_above(marker.above()),
    m_below(marker.below())
{
}

Marker &Marker::operator =(const Marker &marker)
{
    setParent(marker.parent());
    m_offset = marker.offset();
    m_width = marker.width();
    m_height = marker.height();
    m_id = marker.id();
    m_above = marker.above();
    m_below = marker.below();

    return *this;
}

double Marker::distance() const
{
    return gMarkerParams[id()].height() * FOCAL_DISTANCE / (height() * PIXEL_HEIGHT);
}
