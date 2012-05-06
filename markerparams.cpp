#include "markerparams.h"

MarkerParams::MarkerParams(QObject *parent) :
    QObject(parent),
    m_height(0.0),
    m_range(qRgb(0, 0, 0), qRgb(0, 0, 0)),
    m_id(MARKER_NULL),
    m_position(QPair<qreal, qreal>(0.0, 0.0))
{
}

MarkerParams::MarkerParams(const MarkerParams &params) :
    QObject(params.parent()),
    m_height(params.height()),
    m_range(params.range()),
    m_id(params.id()),
    m_name(params.name()),
    m_position(params.position())
{

}

void MarkerParams::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void MarkerParams::setMinR(const QString& minR)
{
    QRgb newMin = qRgb((int)minR.toDouble(), qGreen(m_range.first), qBlue(m_range.first));
    if (newMin != m_range.first) {
        m_range.first = newMin;
        emit minRChanged();
    }
}

void MarkerParams::setMinG(const QString& minG)
{
    QRgb newMin = qRgb(qRed(m_range.first), (int)minG.toDouble(), qBlue(m_range.first));
    if (newMin != m_range.first) {
        m_range.first = newMin;
        emit minGChanged();
    }
}

void MarkerParams::setMinB(const QString& minB)
{
    QRgb newMin = qRgb(qRed(m_range.first), qGreen(m_range.first), (int)minB.toDouble());
    if (newMin != m_range.first) {
        m_range.first = newMin;
        emit minBChanged();
    }
}

void MarkerParams::setMaxR(const QString& maxR)
{
    QRgb newMax = qRgb((int)maxR.toDouble(), qGreen(m_range.second), qBlue(m_range.second));
    if (newMax != m_range.second) {
        m_range.second = newMax;
        emit maxRChanged();
    }
}

void MarkerParams::setMaxG(const QString& maxG)
{
    QRgb newMax = qRgb(qRed(m_range.second), (int)maxG.toDouble(), qBlue(m_range.second));
    if (newMax != m_range.second) {
        m_range.second = newMax;
        emit maxGChanged();
    }
}

void MarkerParams::setMaxB(const QString& maxB)
{
    QRgb newMax = qRgb(qRed(m_range.second), qGreen(m_range.second), (int)maxB.toDouble());
    if (newMax != m_range.second) {
        m_range.second = newMax;
        emit maxBChanged();
    }
}

void MarkerParams::setRange(const QPair<QRgb, QRgb> &range)
{
    bool isMinRChanged = qRed(range.first) != qRed(m_range.first);
    bool isMinGChanged = qGreen(range.first) != qGreen(m_range.first);
    bool isMinBChanged = qBlue(range.first) != qBlue(m_range.first);
    if (m_range != range)
        m_range = range;

    if (isMinRChanged)
        emit minRChanged();
    if (isMinGChanged)
        emit minGChanged();
    if (isMinBChanged)
        emit minBChanged();
}

void MarkerParams::setId(MarkerParams::MarkerId id)
{
    if (id != m_id) {
        m_id = id;
        emit nameChanged();
    }
}

void MarkerParams::setPosition(const QPair<qreal, qreal> &position)
{
    m_position = position;
}

MarkerParams &MarkerParams::operator =(const MarkerParams &params)
{
    setParent(params.parent());
    m_height = params.height();
    m_range = params.range();
    m_id = params.id();
    m_name = params.name();
    m_position = params.position();

    return *this;
}

// Initialize our markers
QList<QObject*> gMarkerParams;

void initMarkerParams()
{
    // blue marker
    MarkerParams *marker = new MarkerParams();
    marker->setHeight(0.1);
    marker->setId(MARKER_BLUE);
    marker->setName("blue");
    QRgb rangeMin = qRgb(0, 80, 105);
    QRgb rangeMax = qRgb(70, 115, 160);
    marker->setRange(QPair<QRgb, QRgb>(rangeMin, rangeMax));
    marker->setPosition(QPair<qreal, qreal>(0.0, 3.0));
    gMarkerParams.append(marker);

    // green marker
    marker = new MarkerParams();
    marker->setHeight(0.1);
    marker->setId(MARKER_GREEN);
    marker->setName("green");
    rangeMin = qRgb(0, 115, 27);
    rangeMax = qRgb(77, 187, 124);
    marker->setRange(QPair<QRgb, QRgb>(rangeMin, rangeMax));
    marker->setPosition(QPair<qreal, qreal>(0.0, 2.0));
    gMarkerParams.append(marker);

    // red marker
    marker = new MarkerParams();
    marker->setHeight(0.1);
    marker->setId(MARKER_RED);
    marker->setName("red");
    rangeMin = qRgb(130, 20, 0);
    rangeMax = qRgb(255, 111, 255);
    marker->setRange(QPair<QRgb, QRgb>(rangeMin, rangeMax));
    marker->setPosition(QPair<qreal, qreal>(0.5, 4.430));
    gMarkerParams.append(marker);
}

#include <QDebug>

MarkerParams findMarkerParams(MarkerParams::MarkerId id)
{
    foreach (QObject *obj, gMarkerParams) {
        MarkerParams* params = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(params);
        if (params->id() == id) {
            return *params;
        }
    }

    qDebug() << "Could not find marker id:" << id;
    return MarkerParams();
}
