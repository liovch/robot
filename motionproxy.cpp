#include "motionproxy.h"

MotionProxy::MotionProxy(QObject *parent) :
    QObject(parent),
    m_socket(0)
{
    QBluetoothAddress address("00:12:02:28:03:34");
    m_socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket, this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(m_socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(error(QBluetoothSocket::SocketError)));
    m_socket->connectToService(address, QBluetoothUuid::SerialPort);
}

void MotionProxy::motionUpdate(const Movement &m)
{
    Q_UNUSED(m);
}

void MotionProxy::connected()
{
    qDebug() << "Bluetooth Connected!";
}

void MotionProxy::error(QBluetoothSocket::SocketError error)
{
    qDebug() << "Bluetooth Error:" << error;
}
