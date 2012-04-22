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

void Robot::move(qreal turn, qreal forward)
{
    // turn, and add randomness to the turning command
    m_angle += turn;
    m_angle += gRandom.gauss(0.0, m_noiseTurn);
    m_angle = fmod(m_angle, 2.0 * M_PI);

    // move, and add randomness to the motion command
    qreal distance = forward;
    distance += gRandom.gauss(0.0, m_noiseForward);

    m_position.first += qCos(m_angle) * distance;
    m_position.second += qSin(m_angle) * distance;

    // TODO: Do we need a cyclic truncate?
}

qreal Robot::measurementProbability(const QList<qreal> &measurementList) const
{
    Q_ASSERT(measurementList.count() == gMarkerParams.count());

    QList<qreal>::const_iterator it(measurementList.begin());
    bool isMeasurementEmpty = true;
    qreal probability = 1.0;
    foreach (QObject *obj, gMarkerParams) {
        MarkerParams *params = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(params);

        qreal measurement = *it; it++;
        if (qFuzzyCompare(measurement, 0.0))
            continue;
        isMeasurementEmpty = false;

        // TODO: Maybe use "false marker probability" instead of 0.0 ?
        if (!isMarkerVisible(params->position()))
            return 0.0;

        qreal distance = qSqrt(qPow((m_position.first - params->position().first), 2) + qPow((m_position.second - params->position().second), 2));
        probability *= gRandom.gaussian(distance, m_noiseSense, measurement);
    }

    return isMeasurementEmpty ? 0.0 : probability; // TODO: Use "can't see a marker" probability instead of 0.0
}

bool Robot::isMarkerVisible(const QPair<qreal, qreal> &markerPosition) const
{
    qreal angle = qAtan2(markerPosition.first - m_position.first, markerPosition.second - m_position.second) + 2.0 * M_PI; // TODO: make sure the qAtan2 range is [-pi; pi]
    angle = fmod(angle, 2.0 * M_PI);

    return (qAbs(angle - m_angle) <= gCamera.angleOfView());
}

