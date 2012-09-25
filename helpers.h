#ifndef HELPERS_H
#define HELPERS_H

#include <qmath.h>
#include "settings.h"

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

inline qreal azimuthToAngle(qreal azimuth)
{
    return (azimuth - ANGLE_AZIMUTH_OFFSET) * M_PI / 180.0;
}

#endif // HELPERS_H
