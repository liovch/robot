#include "motionproxy.h"
#include <qbluetoothsocket.h>

MotionProxy::MotionProxy(QObject *parent) :
    QObject(parent),
    m_socket(0)
{
}

void MotionProxy::motionUpdate(const Movement &m)
{
    Q_UNUSED(m);
}
