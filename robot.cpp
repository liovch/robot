#include "robot.h"
#include <qmath.h>
#include "markerparams.h"

Robot::Robot(QObject *parent) :
    QObject(parent),
    m_position(0.0, 0.0),
    m_angle(0.0),
    m_noiseTurn(0.0),
    m_noiseForward(0.0),
    m_noiseSense(0.0)
{
}

Robot::Robot(const Robot &robot) :
    QObject(robot.parent()),
    m_position(robot.m_position),
    m_angle(robot.m_angle),
    m_noiseTurn(robot.m_noiseTurn),
    m_noiseForward(robot.m_noiseForward),
    m_noiseSense(robot.m_noiseSense)
{
}

void Robot::setPosition(const QPair<qreal, qreal> &position)
{
    m_position = position;
}

void Robot::setAngle(qreal angle)
{
    m_angle = angle;
}

void Robot::setNoiseForward(qreal noise)
{
    m_noiseForward = noise;
}

void Robot::setNoiseTurn(qreal noise)
{
    m_noiseTurn = noise;
}

void Robot::setNoiseSense(qreal noise)
{
    m_noiseSense = noise;
}

QList<qreal> Robot::sense() const
{
    QList<qreal> listDistances;
    foreach (QObject *obj, gMarkerParams) {
        MarkerParams *params = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(params);
        qreal distance = qSqrt(qPow((m_position.first - params->position().first), 2) + qPow((m_position.second - params->position().second), 2));
        //TODO: distance += random.gauss(0.0, self.sense_noise)
        listDistances.append(distance);
    }

    return listDistances;
}

void Robot::move(qreal turn, qreal forward)
{
    // turn, and add randomness to the turning command
    m_angle += turn;
    // TODO: m_angle += random.gauss(0.0, self.turn_noise)

    m_angle = fmod(m_angle, 2.0 * M_PI);

    // move, and add randomness to the motion command
    qreal distance = forward; //TODO: + random.gauss(0.0, self.forward_noise)
    m_position.first += qCos(m_angle) * distance;
    m_position.second += qSin(m_angle) * distance;

    // TODO: Do we need a cyclic truncate?
}
