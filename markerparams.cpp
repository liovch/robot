#include "markerparams.h"

#define MARKER_SIZE 0.15f // m

MarkerParams::MarkerParams(QObject *parent) :
    QObject(parent),
    m_id(MARKER_NULL),
    m_x(0.0),
    m_y(0.0),
    m_z(0.0)
{
}

MarkerParams::MarkerParams(const MarkerParams &params) :
    QObject(params.parent()),
    m_id(params.id()),
    m_x(params.x()),
    m_y(params.y()),
    m_z(params.z())
{
}

qreal MarkerParams::size()
{
    return MARKER_SIZE;
}

MarkerParams &MarkerParams::operator =(const MarkerParams &params)
{
    setParent(params.parent());
    m_id = params.id();
    m_x = params.x();
    m_y = params.y();
    m_z = params.z();

    return *this;
}

// Initialize our markers
QMap<int, MarkerParams> gMarkerParams;

void initMarkerParams()
{
    // TODO: Set proper marker params
    MarkerParams params;

    // TODO: Find out marker ids
    params.setId(9);
    params.setPosition(2.025, 1.645, 2.6);
    gMarkerParams.insert(params.id(), params);

    params.setId(0);
    params.setPosition(1.340, 1.945, 2.6);
    gMarkerParams.insert(params.id(), params);

    params.setId(1);
    params.setPosition(1.340, 0.750, 2.6);
    gMarkerParams.insert(params.id(), params);

    params.setId(2);
    params.setPosition(0.450, 1.200, 2.6);
    gMarkerParams.insert(params.id(), params);
}
