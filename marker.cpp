#include "marker.h"
#include "camera.h"

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

qreal Marker::distance() const
{
    return gCamera.distance(*this);
}
