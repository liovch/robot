#include "random.h"
#include <qmath.h>
#include <QTime>

#ifndef RAND_MAX
#define	RAND_MAX 2147483647 // TODO: Check if this is correct on Harmattan
#endif

Random gRandom;

Random::Random(QObject *parent) :
    QObject(parent),
    isNextGauss(false)
{
    // Create seed for the random
    // That is needed only once on application startup
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}

qreal Random::random() const
{
    return qrand() / ((qreal)RAND_MAX + 1.0);
}

qreal Random::gauss(qreal mu, qreal sigma)
{
    // This code is taken from Python's random.py
    //# When x and y are two variables from [0, 1), uniformly
    //# distributed, then
    //#
    //#    cos(2*pi*x)*sqrt(-2*log(1-y))
    //#    sin(2*pi*x)*sqrt(-2*log(1-y))
    //#
    //# are two *independent* variables with normal distribution
    //# (mu = 0, sigma = 1).
    //# (Lambert Meertens)
    //# (corrected version; bug discovered by Mike Miller, fixed by LM)

    //# Multithreading note: When two threads call this function
    //# simultaneously, it is possible that they will receive the
    //# same return value.  The window is very small though.  To
    //# avoid this, you have to use a lock around all calls.  (I
    //# didn't want to slow this down in the serial case by using a
    //# lock here.)

    qreal z;
    if (isNextGauss) {
        z = nextGauss;
        isNextGauss = false;
    } else {
        qreal x2pi = random() * M_PI * 2.0;
        qreal g2rad = qSqrt(-2.0 * qLn(1.0 - random()));
        z = qCos(x2pi) * g2rad;
        nextGauss = qSin(x2pi) * g2rad;
        isNextGauss = true;
    }
    return mu + z * sigma;
}

qreal Random::gaussian(qreal mu, qreal sigma, qreal x) const
{
    qreal sigma2 = sigma * sigma;
    return qExp(- (mu - x) * (mu - x) / sigma2 / 2.0) / qSqrt(2.0 * M_PI * sigma2);
}
