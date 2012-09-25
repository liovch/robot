#ifndef PARTICLEFILTER_H
#define PARTICLEFILTER_H

#include <QObject>
#include <QVector>
#include "robot.h"
#include "sensordata.h"

class ParticleFilter : public QObject
{
    Q_OBJECT
public:
    explicit ParticleFilter(QObject *parent = 0);
    
    void init(size_t N, qreal maxPositionX, qreal maxPositionY);

    QVector<Robot> particles() const { return m_particles; }
    const Robot& estimatedPosition() const { return m_bestParticle; }

signals:
    // Note: This is emited every time particles changed
    // both after move and resample updates.
    void particlesUpdated(const QVector<Robot>& particles);

    // this is emited after resample operation
    void estimatedPositionChanged(const Robot& robot);

    // this is emited after move operation
    void particlesMoved();

public slots:
    void turn(qreal angle); // [0; 2*PI)
    void move(qreal distance);
    void resample(const SensorData& data);

private:
    QVector<Robot> m_particles;
    Robot m_bestParticle;
};

#endif // PARTICLEFILTER_H
