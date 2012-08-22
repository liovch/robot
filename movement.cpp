#include "movement.h"
#include <qmath.h>

Movement::Movement(QObject *parent) :
    QObject(parent),
    m_turn(0.0),
    m_forward(0.0)
{
}

Movement::Movement(const Movement &m):
    QObject(m.parent()),
    m_turn(m.turn()),
    m_forward(m.forward())
{
}

Movement::Movement(qreal turn, qreal forward) :
    m_turn(turn),
    m_forward(forward)
{
}

void Movement::setTurn(qreal turn)
{
    m_turn = fmod(turn, 2.0 * M_PI);
}

Movement &Movement::operator =(const Movement &m)
{
    m_turn = m.turn();
    m_forward = m.forward();

    return *this;
}
