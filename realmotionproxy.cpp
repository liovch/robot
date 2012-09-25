#include "realmotionproxy.h"
#include <QTimer>
#include "helpers.h"
#include "settings.h"

#define CHECK_IF_STOPPED_INTERVAL 500

RealMotionProxy::RealMotionProxy(QObject *parent) :
    MotionProxy(parent),
    m_socket(0),
    m_isEncoderReadingValid(false),
    m_encoderReadingLeft(0),
    m_encoderReadingRight(0),
    m_targetReadingLeft(0),
    m_targetReadingRight(0),
    m_status(MotionStatusStopped)
{
    QBluetoothAddress address("00:12:02:28:03:34");
    m_socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket, this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(error(QBluetoothSocket::SocketError)));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(bluetoothDataReceived()));
    m_socket->connectToService(address, QBluetoothUuid::SerialPort);
}

void RealMotionProxy::turnRequest(qreal angle)
{
    if (m_status != MotionStatusStopped) {
        qDebug() << "turnRequest failed: current status is not stopped";
        return;
    }

    Q_ASSERT(angle >= 0.0 && angle < M_PI * 2.0);
    if (qFuzzyCompare(angle, (qreal)0.0)) {
        // no motion is required
        emit turnFinished(0.0);
        return;
    }

    // TODO: For now always turn counter-clockwise
    // TODO: Change to use clockwise turn
    qreal encoderReadings = convertAngleToEncoderReading(angle);
    m_isEncoderReadingValid = false; // reset it just in case
    m_targetReadingLeft = quint16(encoderReadings + 0.5);
    m_targetReadingRight = quint16(encoderReadings + 0.5);
    qDebug() << "Starting turn left movement";
    m_status = MotionStatusTurning;
    m_socket->putChar('a'); // turn left (counter-clockwise)

    // TODO: Need to periodically check robot is actually moving
}

void RealMotionProxy::moveRequest(qreal distance)
{
    if (m_status != MotionStatusStopped) {
        qDebug() << "moveRequest failed: current status is not stopped";
        return;
    }

    if (qFuzzyCompare(distance, (qreal)0.0)) {
        qDebug() << "No motion is required";
        emit moveFinished(0.0);
        return;
    }

    qreal encoderReadings = convertDistanceToEncoderReading(distance);
    m_isEncoderReadingValid = false; // reset it just in case
    m_targetReadingLeft = quint16(encoderReadings + 0.5);
    m_targetReadingRight = quint16(encoderReadings + 0.5);
    qDebug() << "Starting forward movement";
    m_status = MotionStatusMovingForward;
    m_socket->putChar('w'); // move forward
    // TODO: Robot can also move backwards

    // TODO: Need to periodically check robot is actually moving
}

void RealMotionProxy::connected()
{
    qDebug() << "Bluetooth has connected successfully";

    if (m_isReady)
        return;

    m_isReady = true;
    emit ready();
}

void RealMotionProxy::error(QBluetoothSocket::SocketError error)
{
    qDebug() << "Bluetooth Error:" << error;
}

void RealMotionProxy::bluetoothDataReceived()
{
    qint64 bytesAvailable = m_socket->bytesAvailable();
    //qDebug() << "Bytes available on Bluetooth:" << bytesAvailable;
    while (bytesAvailable >= 4) {
        quint16 encoderReadingLeft, encoderReadingRight;
        if (m_socket->read((char*)&encoderReadingLeft, 2) != 2 ||
            m_socket->read((char*)&encoderReadingRight, 2) != 2) {
            qDebug() << "Error: Failed to read wheel encoders data";
            break;
        }
        bytesAvailable -= 4;

        //qDebug() << "Wheel encoders data:" << encoderReadingLeft << encoderReadingRight;
        if (m_status == MotionStatusStopped) {
            qDebug() << "Warning: Wheels are moving while robot is in the stopped state";
            continue;
        }

        if (!m_isEncoderReadingValid) {
            m_encoderReadingLeft = encoderReadingLeft;
            m_encoderReadingRight = encoderReadingRight;
            m_isEncoderReadingValid = true;
        }

        m_lastReadingLeft = encoderReadingLeft;
        m_lastReadingRight = encoderReadingRight;
        m_isEncoderUpdateReceived = true;

        if (m_status != MotionStatusStopping &&
            encoderReadingLeft - m_encoderReadingLeft >= m_targetReadingLeft &&
            encoderReadingRight - m_encoderReadingRight >= m_targetReadingRight) {
            qDebug() << "Switching to stopped state" << encoderReadingLeft << encoderReadingRight;

            if (m_status == MotionStatusTurning) {
                qDebug() << "Angle turned so far:" << 180.0 / M_PI * convertEncoderReadingToAngle((encoderReadingLeft + encoderReadingRight) / 2 - (m_encoderReadingLeft + m_encoderReadingRight) / 2);
            } else if (m_status == MotionStatusMovingForward) {
                qDebug() << "Distance traveled so far:" << convertEncoderReadingToDistance((encoderReadingLeft + encoderReadingRight) / 2 - (m_encoderReadingLeft + m_encoderReadingRight) / 2);
            }

            m_statusSaved = m_status;
            m_status = MotionStatusStopping;
            m_socket->putChar('x'); // stop robot motors
            // TODO: Convert result wheel encoders values to Movement and emit finishedMotionUpdate.

            // Wait for the robot to stop moving
            // TODO: Maybe we should start this timer every time we start moving.
            //       This way we can periodically check robot is actually moving and
            //       check for movement failed condition.
            m_isEncoderUpdateReceived = false;
            QTimer::singleShot(CHECK_IF_STOPPED_INTERVAL, this, SLOT(checkIfStopped()));
        }
    }
}

// angle * Radius_Robot = Length_Rotated
// Length_Rotated = PI * Diameter_Wheel * Encoder_Reading / Readings_Per_Cycle
// Encoder_Reading = angle * Radius_Robot * Readings_Per_Cycle / (PI * Diameter_Wheel)
qreal RealMotionProxy::convertAngleToEncoderReading(qreal angle)
{
    return angle * ROBOT_PLATFORM_RADIUS * ENCODER_WHEEL_READINGS_PER_CYCLE / (M_PI * WHEEL_DIAMETER);
}

qreal RealMotionProxy::convertDistanceToEncoderReading(qreal distance)
{
    return distance * ENCODER_WHEEL_READINGS_PER_CYCLE / (M_PI * WHEEL_DIAMETER);
}

qreal RealMotionProxy::convertEncoderReadingToAngle(qreal encoderReading)
{
    return encoderReading * M_PI * WHEEL_DIAMETER / (ROBOT_PLATFORM_RADIUS * ENCODER_WHEEL_READINGS_PER_CYCLE);
}

qreal RealMotionProxy::convertEncoderReadingToDistance(qreal encoderReading)
{
    return encoderReading * M_PI * WHEEL_DIAMETER / ENCODER_WHEEL_READINGS_PER_CYCLE;
}

void RealMotionProxy::checkIfStopped()
{
    if (m_isEncoderUpdateReceived) {
        qDebug() << "Robot is still moving" << m_lastReadingLeft << m_lastReadingRight;
        // TODO: Check if we've waited too long?
        m_isEncoderUpdateReceived = false;
        QTimer::singleShot(CHECK_IF_STOPPED_INTERVAL, this, SLOT(checkIfStopped()));
        return;
    }

    Q_ASSERT(m_isEncoderReadingValid);
    qDebug() << "Robot has stopped" << m_lastReadingLeft << m_lastReadingRight;
    m_lastReadingLeft -= m_encoderReadingLeft;
    m_lastReadingRight -= m_encoderReadingRight;
    qreal encoderReading = (m_lastReadingLeft + m_lastReadingRight) / (qreal)2.0;

    // Convert wheel encoders data into angle or distance.
    if (m_statusSaved == MotionStatusTurning) {
        qreal angle = convertEncoderReadingToAngle(encoderReading);
        angle = normalizeAngle(angle);
        qDebug() << "Total angle turned" << angle * 180.0 / M_PI;
        emit turnFinished(angle);
    } else if (m_statusSaved == MotionStatusMovingForward) {
        qreal distance = convertEncoderReadingToDistance(encoderReading);
        qDebug() << "Total distance moved" << distance;
        emit moveFinished(distance);
    } else {
        qDebug() << "Invalid motion status recorded" << m_statusSaved;
    }
    m_status = MotionStatusStopped;
}
