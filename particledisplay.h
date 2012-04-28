#ifndef PARTICLEDISPLAY_H
#define PARTICLEDISPLAY_H

#include <QDeclarativeItem>
#include "robot.h"

class ParticleDisplay : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit ParticleDisplay(QDeclarativeItem *parent = 0);
    
    void setWorldSize(qreal maxPosition);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
    
public slots:
    void setParticles(const QVector<Robot>& particles);

private:
    qreal m_maxPosition;
    QVector<Robot> m_particles;
    qreal m_particleSize;
    qreal m_directionVectorSize;
    qreal m_markerSize;
};

#endif // PARTICLEDISPLAY_H
