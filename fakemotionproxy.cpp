#include "fakemotionproxy.h"

FakeMotionProxy::FakeMotionProxy(QObject *parent) :
    MotionProxy(parent)
{
}

void FakeMotionProxy::motionUpdate(const Movement &m)
{
    emit finishedMotionUpdate(m);
}
