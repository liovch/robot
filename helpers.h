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

#endif // HELPERS_H
