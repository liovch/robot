#ifndef MOTIONPROXY_H
#define MOTIONPROXY_H

#include <QObject>

// ** Implementations of this abstract class:
//    - receive motion update from MotionPlanner;
//    - convert them ands sends commands to the device;
//    - wait for device to complete motion commands;
//    - report completition status;
class MotionProxy : public QObject
{
    Q_OBJECT
public:
    explicit MotionProxy(QObject *parent = 0): QObject(parent), m_isReady(false) { }

    bool isReady() const { return m_isReady; }
    
signals:
    // emited when the object becomes ready to receive data
    void ready();

    // Emited just after robot has finished moveRequest or failed
    // to perform move request.
    // Parameter indicates the actual distance robot has traveled.
    void turnFinished(qreal angle);
    void moveFinished(qreal distance);

    // Emited when robot was unable to perform the requested movement.
    // This should indicate that robot is stuck.
    // moveFinished or turnFinished is still emited in this case.
    void motionRequestFailed();
    
public slots:
    // Request motion update to perform on the robot.
    // This is connected to motion planner.
    virtual void turnRequest(qreal angle) = 0;
    virtual void moveRequest(qreal distance) = 0;

protected:
    bool m_isReady;
};

#endif // MOTIONPROXY_H
