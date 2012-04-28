#include "particlefilter.h"
#include "random.h"

ParticleFilter::ParticleFilter(QObject *parent) :
    QObject(parent)
{
}

void ParticleFilter::init(size_t N, qreal maxPosition)
{
    m_particles.resize(N);
    foreach (Robot r, m_particles) {
        r.random(maxPosition);
        // TODO: Set noise
    }
}

void ParticleFilter::move(qreal turn, qreal forward)
{
    foreach (Robot r, m_particles) {
        r.move(turn, forward);
    }
}

void ParticleFilter::resample(const QList<qreal> &measurementList)
{
    size_t N = m_particles.size();
    QVector<qreal> weights(N);

    qreal maxWeight = 0.0;
    for (size_t n = 0; n < N; n++) {
        weights[n] = m_particles[n].measurementProbability(measurementList);
        if (weights[n] > maxWeight)
            maxWeight = weights[n];
    }

    // Resampling wheel
    QList<Robot> resampled;
    int index = int(Random().random() * N);
    qreal beta = 0.0;
    for (size_t n = 0; n < N; n++) {
        beta += Random().random() * 2.0 * maxWeight;
        while (beta > weights[index]) {
            beta -= weights[index];
            index = (index + 1) % N;
        }
        resampled.append(m_particles[index]);
    }

    size_t n = 0;
    foreach (Robot r, resampled) {
        m_particles[n] = r;
        n++;
    }
}
