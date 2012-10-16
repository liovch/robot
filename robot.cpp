#include "robot.h"
#include <qmath.h>
#include "markerparams.h"
#include "random.h"
#include "camera.h"
#include "helpers.h"
#include <QDebug>

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

void Robot::random(qreal maxPositionX, qreal maxPositionY)
{
    m_position.first = gRandom.random() * maxPositionX;
    m_position.second = gRandom.random() * maxPositionY;
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

QList<qreal> Robot::sense() const
{
    QList<qreal> listDistances;
    foreach (MarkerParams params, gMarkerParams) {
        qreal distance = qSqrt(qPow((m_position.first - params.x()), 2) + qPow((m_position.second - params.y()), 2) + params.z() * params.z());
        distance += gRandom.gauss(0.0, m_noiseSense);
        listDistances.append(distance);
    }

    return listDistances;
}

void Robot::move(qreal distance)
{
    // move, and add randomness to the motion command
    distance += gRandom.gauss(0.0, m_noiseForward);

    m_position.first += qCos(m_angle) * distance;
    m_position.second += qSin(m_angle) * distance;

    // TODO: Do we need a cyclic truncate for position?
}

void Robot::turn(qreal angle)
{
    // turn, and add randomness to the turning command
    m_angle += angle;
    m_angle += gRandom.gauss(0.0, m_noiseTurn);
    m_angle = fmod(m_angle, 2.0 * M_PI);
}

qreal Robot::measurementProbability(const SensorData& data) const
{
    // TODO: Use "can't see a marker" probability instead?
    qreal probability = 1.0;
    foreach (Marker m, data.m_markers) {
        MarkerParams params = gMarkerParams.value(m.id);

        // TODO: Maybe use "false marker probability" instead of 0.0 ?
        if (!isMarkerVisible(params.x(), params.y(), params.z()))
            return 0.0;

        // TODO: Take into account: camera is elevated from the ground.
        qreal distance = qSqrt(qPow((m_position.first - params.x()), 2) + qPow((m_position.second - params.y()), 2) + params.z() * params.z());
        qreal gauss = gRandom.gaussian(distance, NOISE_SENSE_DISTANCE, m.distance());
        if (gauss > 1.0) {
            qDebug() << "Gaussian probability is wrong:" << gauss << "mu:" << distance << "sigma:" << m_noiseSense << "x:" << m.distance();
        }
        probability *= gauss;
    }

    qreal gauss = gRandom.gaussian(m_angle, NOISE_SENSE_ANGLE, data.m_angle);
    probability *= gauss;

    return probability;
}

bool Robot::isMarkerVisible(qreal x, qreal y, qreal z) const
{
    // TODO: Check marker visibility
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(z);
    return true;
//    // Note: There's a problem with qAtan2. It should be declared as qAtan2(y, x)
//    qreal angle = qAtan2(x.second - m_position.second, x.first - m_position.first) + 2.0 * M_PI; // TODO: make sure the qAtan2 range is [-pi; pi]
//    angle = fmod(angle, 2.0 * M_PI);

//    return (qAbs(angle - m_angle) <= gCamera.angleOfView());
}

bool Robot::isEveryMarkerVisible() const
{
    foreach (MarkerParams params, gMarkerParams) {
        if (!isMarkerVisible(params.x(), params.y(), params.z()))
            return false;
    }

    return true;
}

Robot &Robot::operator =(const Robot &r)
{
    m_position = r.position();
    m_angle = r.angle();

    return *this;
}

