#ifndef PARTICLEFILTER_H
#define PARTICLEFILTER_H

#include <QObject>
#include <QVector>
#include "robot.h"

class ParticleFilter : public QObject
{
    Q_OBJECT
public:
    explicit ParticleFilter(QObject *parent = 0);
    
    void init(size_t N, qreal maxPosition);

    QVector<Robot> particles() const { return m_particles; }
    Robot calculatePosition() const;

signals:
    void particlesUpdated(const QVector<Robot>& particles);
    void estimatedPosition(const Robot& robot);

public slots:
    void move(const Movement& m);
    void resample(const QList<Marker>& markers);

private:
    QVector<Robot> m_particles;
};

#endif // PARTICLEFILTER_H
