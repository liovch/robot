#ifndef HELPERS_H
#define HELPERS_H

#include <qmath.h>
#include "settings.h"
#include <QDebug>

inline qreal normalizeAngle(qreal angle)
{
    angle = fmod(angle, 2.0 * M_PI);
    if (angle < 0.0)
        angle += 2.0 * M_PI;
    return angle;
}

inline qreal angleDistance(qreal angle1, qreal angle2)
{
    angle1 = normalizeAngle(angle1);
    angle2 = normalizeAngle(angle2);
    qreal distance = qAbs(angle1 - angle2);
    if (distance > M_PI) {
        distance = 2 * M_PI - distance;
    }
    return normalizeAngle(distance);
}

inline bool isAngleInRange(qreal angle, qreal minAngle, qreal maxAngle)
{
    angle = normalizeAngle(angle);
    minAngle = normalizeAngle(minAngle);
    maxAngle = normalizeAngle(maxAngle);
    return angle >= minAngle && angle < maxAngle;
}

inline qreal scaleAngleInRange(qreal angle, qreal minAngle, qreal maxAngle, qreal minValue, qreal maxValue)
{
    return minValue + (angle - minAngle) * (maxValue - minValue) / (maxAngle - minAngle);
}

#define MAGNETOMETER_ANGLE_0   -0.28
#define MAGNETOMETER_ANGLE_90  1.20
#define MAGNETOMETER_ANGLE_180 2.88
#define MAGNETOMETER_ANGLE_270 -1.86

inline qreal magnetometerToAngle(qreal x, qreal y)
{
    qreal angle = qAtan2(y, x);

    // TODO: Measure magnetometer values for each of the angles
    //       Convert values approprietly.
    if (isAngleInRange(angle, MAGNETOMETER_ANGLE_0, MAGNETOMETER_ANGLE_90)) {
        angle = scaleAngleInRange(angle, MAGNETOMETER_ANGLE_0, MAGNETOMETER_ANGLE_90, 0, M_PI_2);
    } else if (isAngleInRange(angle, MAGNETOMETER_ANGLE_90, MAGNETOMETER_ANGLE_180)) {
        angle = scaleAngleInRange(angle, MAGNETOMETER_ANGLE_90, MAGNETOMETER_ANGLE_180, M_PI_2, M_PI);
    } else if (isAngleInRange(angle, MAGNETOMETER_ANGLE_180, MAGNETOMETER_ANGLE_270)) {
        angle = scaleAngleInRange(angle, MAGNETOMETER_ANGLE_180, MAGNETOMETER_ANGLE_270, M_PI, M_PI + M_PI_2);
    } else if (isAngleInRange(angle, MAGNETOMETER_ANGLE_270, MAGNETOMETER_ANGLE_0)) {
        angle = scaleAngleInRange(angle, MAGNETOMETER_ANGLE_270, MAGNETOMETER_ANGLE_0, M_PI + M_PI_2, M_PI * 2.0);
    } else {
        qDebug() << "Invalid magnetometer angle:" << angle;
    }
    return normalizeAngle(angle);
}

#endif // HELPERS_H
