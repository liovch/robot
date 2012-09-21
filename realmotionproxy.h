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

// ** This class receives motion request from MotionPlanner
//    and calculates the distance each robot wheel must travel to
//    perform the requested motion (in wheel encoder ticks).
//    The motion command is then sent to the robot.
//    This class then receives wheel encoder updates sent by Arduino
//    and calculates the actual distance traveled by the robot.
//    As soon as robot have travelled the requested distance stop command
//    is sent to the robot and we wait for the robot to stop movement.
//    The actual distance travelled by the robot is then calculated
//    and emitted with the finished signal.
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
    // Request motion to perform on the robot.
    // This is connected to motion planner.
    void turnRequest(qreal angle);
    void moveRequest(qreal distance);

public slots:
    // bluetooth signals
    void connected();
    void error(QBluetoothSocket::SocketError error);
    void bluetoothDataReceived();

private:
    static qreal convertAngleToEncoderReading(qreal angle);
    static qreal convertDistanceToEncoderReading(qreal distance);
    static qreal convertEncoderReadingToAngle(qreal encoderReading);
    static qreal convertEncoderReadingToDistance(qreal encoderReading);

    QBluetoothSocket *m_socket;

    // Data from wheel encoders.
    bool m_isEncoderReadingValid;
    quint16 m_encoderReadingLeft;
    quint16 m_encoderReadingRight;

    quint16 m_targetReadingLeft;
    quint16 m_targetReadingRight;

    bool m_isEncoderUpdateReceived;
    quint16 m_lastReadingLeft;
    quint16 m_lastReadingRight;

    enum Status { MotionStatusStopped, MotionStatusStopping, MotionStatusTurning, MotionStatusMovingForward };
    Status m_status; // current status
    Status m_statusSaved; // need to know what robot was doing before it stopped

private slots:
    void checkIfStopped();
};

#endif // REALMOTIONPROXY_H
