#include "realmotionproxy.h"
#include "settings.h"

RealMotionProxy::RealMotionProxy(QObject *parent) :
    MotionProxy(parent),
    m_socket(0),
    m_encoderReadingLeft(0),
    m_encoderReadingRight(0),
    m_targetReadingLeft(0),
    m_targetReadingRight(0),
    m_queuedForwardMovement(0.0),
    m_status(MotionStatusStopped)
{
    QBluetoothAddress address("00:12:02:28:03:34");
    m_socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket, this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(error(QBluetoothSocket::SocketError)));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(bluetoothDataReceived()));
    m_socket->connectToService(address, QBluetoothUuid::SerialPort);
}

void RealMotionProxy::motionUpdate(const Movement &m)
{
    m_queuedForwardMovement = m.forward();

    // Convert turn angle to motor command
    Q_ASSERT(m.turn() >= 0 && m.turn() < M_PI * 2.0);
    if (qFuzzyCompare(m.turn(), 0)) {
        moveForward();
    } else {
        // TODO: For now always turn counter-clockwise
        // TODO: Change to use clockwise turn
        qreal encoderReadings = calculateEncoderReadingTurn(m.turn());
        m_targetReadingLeft = m_encoderReadingLeft + quint16(encoderReadings + 0.5);
        m_targetReadingRight = m_encoderReadingRight + quint16(encoderReadings + 0.5);
        m_status = MotionStatusTurning;
        m_socket->putChar('a'); // turn left (counter-clockwise)
    }
}

void RealMotionProxy::connected()
{
    qDebug() << "Bluetooth Connected!";
}

void RealMotionProxy::error(QBluetoothSocket::SocketError error)
{
    qDebug() << "Bluetooth Error:" << error;
}

void RealMotionProxy::bluetoothDataReceived()
{
    if (m_socket->bytesAvailable() < 4)
        return;

    if (m_socket->read((char*)&m_encoderReadingLeft, 2) != 2 ||
        m_socket->read((char*)&m_encoderReadingRight, 2) != 2) {
        qDebug() << "Error: Failed to read wheel encoders data";
    } else {
        qDebug() << "Wheel encoders data:" << m_encoderReadingLeft << m_encoderReadingRight;

        if (m_encoderReadingLeft >= m_targetReadingLeft && m_encoderReadingRight >= m_targetReadingRight) {
            if (m_status == MotionStatusTurning) {
                moveForward();
            } else if (m_status == MotionStatusMovingForward) {
                stop();
            } else {
                // TODO: Is it ok if we ignore some updates while in MotionStatusStopped mode?
                qDebug() << "Ignoring this wheel encoder update";
            }
        }
    }
}

// angle * Radius_Robot = Length_Rotated
// Length_Rotated = PI * Diameter_Wheel * Encoder_Reading / Readings_Per_Cycle
// Encoder_Reading = angle * Radius_Robot * Readings_Per_Cycle / (PI * Diameter_Wheel)
qreal RealMotionProxy::calculateEncoderReadingTurn(qreal angle)
{
    return angle * ROBOT_PLATFORM_RADIUS * ENCODER_WHEEL_READINGS_PER_CYCLE / (M_PI * WHEEL_DIAMETER);
}

qreal RealMotionProxy::calculateEncoderReadingForward(qreal forward)
{
    return forward * ENCODER_WHEEL_READINGS_PER_CYCLE / (M_PI * WHEEL_DIAMETER);
}

void RealMotionProxy::moveForward()
{
    if (qFuzzyCompare(m_queuedForwardMovement, 0.0)) {
        stop();
    } else {
        qreal encoderReadings = calculateEncoderReadingForward(m_queuedForwardMovement);
        m_targetReadingLeft = m_encoderReadingLeft + quint16(encoderReadings + 0.5);
        m_targetReadingRight = m_encoderReadingRight + quint16(encoderReadings + 0.5);
        m_status = MotionStatusMovingForward;
        m_socket->putChar('w'); // move forward
        // TODO: Robot can also move backwards
    }
}

void RealMotionProxy::stop()
{
    m_status = MotionStatusStopped;
    m_socket->putChar('x'); // stop robot motors
    // TODO: Convert result wheel encoders values to Movement and emit finishedMotionUpdate.
}
