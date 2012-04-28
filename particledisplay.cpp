#include "particledisplay.h"
#include <QPainter>
#include <qmath.h>
#include "settings.h"

ParticleDisplay::ParticleDisplay(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_maxPosition(MAX_POSITION), // default world size 10x10 meters
    m_particleSize(2.0), // default particle size in pixels
    m_directionVectorSize(5.0), // default direction vector size in pixels
    m_markerSize(7.0) // default marker size in pixels
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

void ParticleDisplay::setWorldSize(qreal maxPosition)
{
    m_maxPosition = maxPosition;
    update();
}

void ParticleDisplay::setParticles(const QVector<Robot> &particles)
{
    m_particles = particles;
    update();
}

void ParticleDisplay::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
//    painter->scale(1.0, -1.0);
//    painter->translate(1.0, -boundingRect().height());

    // draw markers
    foreach (QObject *obj, gMarkerParams) {
        MarkerParams* params = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(params);

        painter->setBrush(QBrush(QColor(params->name())));
        QPointF p((params->position().first / m_maxPosition) * boundingRect().width(), (params->position().second / m_maxPosition) * boundingRect().height());
        painter->drawRect(QRectF(p.x() - (m_markerSize / 2.0), p.y() - (m_markerSize / 2.0), m_markerSize, m_markerSize));
    }

    painter->setBrush(QBrush(QColor("black")));
    foreach (Robot r, m_particles) {
        QPointF p1((r.position().first / m_maxPosition) * boundingRect().width(), (r.position().second / m_maxPosition) * boundingRect().height());
        painter->drawRect(QRectF(p1.x() - (m_particleSize / 2.0), p1.y() - (m_particleSize / 2.0), m_particleSize, m_particleSize));
        QPointF p2(p1.x() + qCos(r.angle()) * m_directionVectorSize, p1.y() + qSin(r.angle()) * m_directionVectorSize);
        painter->drawLine(p1, p2);
    }
}
