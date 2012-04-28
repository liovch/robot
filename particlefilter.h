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
    void move(qreal turn, qreal forward);
    void resample(const QList<qreal>& measurementList);

    QVector<Robot> particles() const { return m_particles; }

signals:
    
public slots:

private:
    QVector<Robot> m_particles;
};

#endif // PARTICLEFILTER_H
