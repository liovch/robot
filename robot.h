#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include <QList>
#include <QPair>
#include "sensordata.h"

// TODO: Rename this to Particle?
// Robot class could be associated with real Robot sensors.
class Robot : public QObject
{
    Q_OBJECT
public:
    explicit Robot(QObject *parent = 0);
    Robot(const Robot& robot);

    // initialize random robot position and angle
    void random(qreal maxPositionX, qreal maxPositionY);

    QPair<qreal, qreal> position() const { return m_position; }
    qreal angle() const { return m_angle; }

    void setPosition(const QPair<qreal, qreal>& position);
    void setAngle(qreal angle);

    // returns list of distances to each unique marker with added sense noise
    QList<qreal> sense() const;
    void move(qreal distance);
    void turn(qreal angle);

    // TODO: Probably need a separate class for measurement
    // calculates how likely a measurement should be
    // TODO: Change this to a copy of SensorData ?
    qreal measurementProbability(const SensorData& data) const;

    // check if marker is visible from robot's location
    bool isMarkerVisible(qreal x, qreal y, qreal z) const;
    bool isEveryMarkerVisible() const;

    Robot& operator=(const Robot& r);

signals:
    
public slots:
    
private:
    QPair<qreal, qreal> m_position;
    qreal m_angle; // relative to positive x axis counter clockwise

    // noise
    qreal m_noiseTurn;
    qreal m_noiseForward;
    qreal m_noiseSense; // TODO: This is probably proportional to the distance to the object
};

#endif // ROBOT_H
