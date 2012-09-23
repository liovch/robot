#ifndef HELPERS_H
#define HELPERS_H

#include <qmath.h>

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

#endif // HELPERS_H
