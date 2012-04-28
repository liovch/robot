#include "robot.h"
#include <qmath.h>
#include "markerparams.h"
#include "random.h"
#include "camera.h"

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

void Robot::random(qreal maxPosition)
{
    m_position.first = gRandom.random() * maxPosition;
    m_position.second = gRandom.random() * maxPosition;
    m_angle = gRandom.random() * 2.0 * M_PI;

    m_noiseTurn = 0.0;
    m_noiseForward = 0.0;
    m_noiseSense = 0.0;
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
        distance += gRandom.gauss(0.0, m_noiseSense);
        listDistances.append(distance);
    }

    return listDistances;
}

void Robot::move(const Movement &m)
{
    // turn, and add randomness to the turning command
    m_angle += m.turn();
    m_angle += gRandom.gauss(0.0, m_noiseTurn);
    m_angle = fmod(m_angle, 2.0 * M_PI);

    // move, and add randomness to the motion command
    qreal distance = m.forward();
    distance += gRandom.gauss(0.0, m_noiseForward);

    m_position.first += qCos(m_angle) * distance;
    m_position.second += qSin(m_angle) * distance;

    // TODO: Do we need a cyclic truncate?
}

qreal Robot::measurementProbability(const QList<Marker> &markers) const
{
    if (markers.isEmpty())
        return 0.0; // TODO: Use "can't see a marker" probability instead of 0.0

    qreal probability = 1.0;
    foreach (Marker m, markers) {
        MarkerParams params = findMarkerParams(m.id());

        // TODO: Maybe use "false marker probability" instead of 0.0 ?
        if (!isMarkerVisible(params.position()))
            return 0.0;

        qreal distance = qSqrt(qPow((m_position.first - params.position().first), 2) + qPow((m_position.second - params.position().second), 2));
        probability *= gRandom.gaussian(distance, m_noiseSense, m.distance());
    }

    return probability;
}

bool Robot::isMarkerVisible(const QPair<qreal, qreal> &markerPosition) const
{
    qreal angle = qAtan2(markerPosition.first - m_position.first, markerPosition.second - m_position.second) + 2.0 * M_PI; // TODO: make sure the qAtan2 range is [-pi; pi]
    angle = fmod(angle, 2.0 * M_PI);

    return (qAbs(angle - m_angle) <= gCamera.angleOfView());
}

Robot &Robot::operator =(const Robot &r)
{
    m_position = r.position();
    m_angle = r.angle();
    m_noiseTurn = r.noiseTurn();
    m_noiseForward = r.noiseForward();
    m_noiseSense = r.noiseSense();

    return *this;
}

