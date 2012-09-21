#ifndef FAKEMOTIONPROXY_H
#define FAKEMOTIONPROXY_H

#include <QObject>
#include "motionproxy.h"

// ** This class simulates real motion proxy by using a data file.
class FakeMotionProxy : public MotionProxy
{
    Q_OBJECT
public:
    explicit FakeMotionProxy(QObject *parent = 0);
    
    // slots
    // Request motion update to perform on the robot.
    // This is connected to motion planner.
    void turnRequest(qreal angle);
    void moveRequest(qreal distance);

public slots:

private:
};

#endif // REALMOTIONPROXY_H
