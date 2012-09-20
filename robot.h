#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include <QList>
#include <QPair>
#include "marker.h"
#include "movement.h"

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

    qreal noiseForward() const { return m_noiseForward; }
    qreal noiseTurn() const { return m_noiseTurn; }
    qreal noiseSense() const { return m_noiseSense; }

    void setPosition(const QPair<qreal, qreal>& position);
    void setAngle(qreal angle);

    void setNoiseForward(qreal noise);
    void setNoiseTurn(qreal noise);
    void setNoiseSense(qreal noise);

    // returns list of distances to each unique marker with added sense noise
    QList<qreal> sense() const;
    void move(const Movement& m);

    // TODO: Probably need a separate class for measurement
    // calculates how likely a measurement should be
    // TODO: Change this to QList<Marker> ?
    qreal measurementProbability(const QList<Marker>& markers) const;

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
    // TODO: So how do we measure real noise?
    // TODO: Should we move this out and make them global?
    qreal m_noiseTurn;
    qreal m_noiseForward;
    qreal m_noiseSense; // TODO: This is probably proportional to the distance to the object
};

#endif // ROBOT_H
