#include "markerparams.h"

MarkerParams::MarkerParams(QObject *parent) :
    QObject(parent),
    m_height(0.0),
    m_range(qRgb(0, 0, 0), qRgb(0, 0, 0)),
    m_id(MARKER_NULL)
{
}

MarkerParams::MarkerParams(const MarkerParams &params) :
    QObject(params.parent()),
    m_height(params.height()),
    m_range(params.range()),
    m_id(params.id())
{

}

MarkerParams &MarkerParams::operator =(const MarkerParams &params)
{
    setParent(params.parent());
    m_height = params.height();
    m_range = params.range();
    m_id = params.id();

    return *this;
}

// Initialize our markers
QMap<MarkerParams::MarkerId, MarkerParams> gMarkerParams;

void initMarkerParams()
{
    MarkerParams marker;
    marker.setHeight(0.1);

    // blue marker
    marker.setId(MARKER_BLUE);
    QRgb rangeMin = qRgb(0, 80, 105);
    QRgb rangeMax = qRgb(70, 115, 160);
    marker.setRange(QPair<QRgb, QRgb>(rangeMin, rangeMax));
    gMarkerParams.insert(marker.id(), marker);

    // green marker
    marker.setId(MARKER_GREEN);
    rangeMin = qRgb(0, 115, 27);
    rangeMax = qRgb(77, 187, 124);
    marker.setRange(QPair<QRgb, QRgb>(rangeMin, rangeMax));
    gMarkerParams.insert(marker.id(), marker);

    // red marker
    marker.setId(MARKER_RED);
    rangeMin = qRgb(130, 20, 0);
    rangeMax = qRgb(255, 111, 255);
    marker.setRange(QPair<QRgb, QRgb>(rangeMin, rangeMax));
    gMarkerParams.insert(marker.id(), marker);
}
