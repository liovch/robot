#include "realmotionproxy.h"

RealMotionProxy::RealMotionProxy(QObject *parent) :
    QObject(parent),
    m_socket(0),
    m_encoderReadingLeft(0),
    m_encoderReadingRight(0)
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
    Q_UNUSED(m);
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
    }
}
