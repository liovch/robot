#ifndef REALMOTIONPROXY_H
#define REALMOTIONPROXY_H

#include <QObject>
#include "motionproxy.h"

#include <QtMobility/qmobilityglobal.h>
#include <qbluetoothsocket.h>
QTM_BEGIN_NAMESPACE
class QBluetoothSocket;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

// ** This class receives motion update from MotionPlanner
//    and converts it into 2 motor speed commands
//    (1st for rotation and 2nd for forward movement) for the robot
//    as [-1..1] value for each motor.
//    The 1st (rotation) motor speed command is then sent to the robot.
//    This class then receives wheel encoder updates sent by Arduino
//    and calculates the actual distance traveled by the robot.
//    As soon as robot have travelled the requested distance 2nd motor
//    command (for forward movement) is issued.
//    Class then waits for the robot to finish the movement
//    while receiving updates from wheel encoders sent by Arduino via
//    Bluetooth.
//    If for some reason robot is unable to perform the requested motion
//    and no updates from encoders are received for some time,
//    current travelled angle and distance are sent to the particle filter,
//    and the current point in the direction of where robot is heading should
//    be marked as an obstacle (temporary?) on the grid map to prevent motion
//    planner from using same route the next time.
class RealMotionProxy : public MotionProxy
{
    Q_OBJECT
public:
    explicit RealMotionProxy(QObject *parent = 0);
    
    // slots
    // Request motion update to perform on the robot.
    // This is connected to motion planner.
    void motionUpdate(const Movement& m);

public slots:
    // bluetooth signals
    void connected();
    void error(QBluetoothSocket::SocketError error);
    void bluetoothDataReceived();

private:
    static qreal calculateEncoderReadingTurn(qreal angle);
    static qreal calculateEncoderReadingForward(qreal forward);

    QBluetoothSocket *m_socket;

    // Data from wheel encoders.
    quint16 m_encoderReadingLeft;
    quint16 m_encoderReadingRight;

    quint16 m_targetReadingLeft;
    quint16 m_targetReadingRight;

    qreal m_queuedForwardMovement;

    enum Status { MotionStatusStopped, MotionStatusTurning, MotionStatusMovingForward } m_status;

private slots:
    void moveForward();
    void stop();
};

#endif // REALMOTIONPROXY_H
