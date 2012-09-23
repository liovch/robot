#ifndef PARTICLEDISPLAY_H
#define PARTICLEDISPLAY_H

#include <QVector>
#include "robot.h"

class ParticleDisplay : public QObject
{
    Q_OBJECT
public:
    explicit ParticleDisplay(QObject *parent = 0);

signals:
    
public slots:
    void setParticles(const QVector<Robot>& particles);
    void saveImage();

private:
    QVector<Robot> m_particles;
    qreal m_particleSize;
    qreal m_directionVectorSize;
    qreal m_markerSize;
};

#endif // PARTICLEDISPLAY_H
