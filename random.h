#ifndef RANDOM_H
#define RANDOM_H

#include <QObject>

class Random : public QObject
{
    Q_OBJECT
public:
    explicit Random(QObject *parent = 0);
    
    // get the next random number in the range [0.0; 1.0)
    // thread-safe
    // TODO: use Mersenne Twister core generator from random.py
    qreal random() const;

    // mu is the mean, and sigma is the standard deviation.
    // Not thread-safe without a lock around calls.
    qreal gauss(qreal mu, qreal sigma);

    // calculates the probability of x for 1-dim Gaussian with mean mu and var. sigma
    qreal gaussian(qreal mu, qreal sigma, qreal x) const;

signals:
    
public slots:
private:
    bool isNextGauss;
    qreal nextGauss;
};

extern Random gRandom;

#endif // RANDOM_H
