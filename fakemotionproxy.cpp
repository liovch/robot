#include "fakemotionproxy.h"

FakeMotionProxy::FakeMotionProxy(QObject *parent) :
    MotionProxy(parent)
{
}

void FakeMotionProxy::turnRequest(qreal angle)
{
    emit turnFinished(angle);
}

void FakeMotionProxy::moveRequest(qreal distance)
{
    emit moveFinished(distance);
}
