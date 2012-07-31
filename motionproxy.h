#ifndef MOTIONPROXY_H
#define MOTIONPROXY_H

#include <QObject>
#include "movement.h"

// ** Implementations of this abstract class:
//    - receive motion update from MotionPlanner;
//    - convert them ands sends commands to the device;
//    - wait for device to complete motion commands;
//    - report completition status;
class MotionProxy : public QObject
{
    Q_OBJECT
public:
    explicit MotionProxy(QObject *parent = 0): QObject(parent) { }
    
signals:
    // Emited just after robot has finished motionUpdate or failed
    // to perform motion update.
    // Parameter indicates the actual movement robot has performed.
    void finishedMotionUpdate(const Movement& m);

    // Emited when robot was unable to perform the requested motionUpdate.
    // This should indicate that robot is stuck.
    // finishedMotionUpdate is still emited in this case.
    void failedMotionUpdate();
    
public slots:
    // Request motion update to perform on the robot.
    // This is connected to motion planner.
    virtual void motionUpdate(const Movement& m) = 0;
};

#endif // MOTIONPROXY_H
