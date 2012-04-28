#include "movementprovider.h"
#include <qmath.h>

//movements =    [[0.0, 0.0],
//                [0.0, 0.2],
//                [0.0, 0.2],
//                [0.0, 0.2],
//                [0.0, 0.2],
//                [0.0, 0.2],
//                [-pi / 4, 0.0],
//                [0.0, 0.2],
//                [0.0, 0.2],
//                [pi / 4, 0.0],
//                [0.0, 0.2],
//                [0.0, 0.2],
//                [0.0, 0.2],
//                [-pi / 4.0, 0.0],
//                [0.0, 0.2],
//                [-pi / 4.0, 0.0]]

MovementProvider::MovementProvider(QObject *parent) :
    QObject(parent)
{
    m_movements.append(Movement(0.0, 0.0));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(-M_PI / 4.0, 0.0));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(M_PI / 4.0, 0.0));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(-M_PI / 4.0, 0.0));
    m_movements.append(Movement(0.0, 0.2));
    m_movements.append(Movement(-M_PI / 4.0, 0.0));
}

void MovementProvider::requestNextMovement()
{
    if (m_movements.isEmpty())
        return;
    emit nextMovement(m_movements.takeFirst());
}
