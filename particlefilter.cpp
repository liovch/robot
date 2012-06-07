#include "particlefilter.h"
#include "random.h"
#include "settings.h"
#include <QDebug>

ParticleFilter::ParticleFilter(QObject *parent) :
    QObject(parent)
{
}

void ParticleFilter::init(size_t N, qreal maxPosition)
{
    m_particles.resize(N);
    for (int n = 0; n < m_particles.size(); n++) {
        do {
            m_particles[n].random(maxPosition);
        } while (!m_particles[n].isEveryMarkerVisible());

        // TODO: Maybe noise could be global parameter
        m_particles[n].setNoiseForward(NOISE_FORWARD);
        m_particles[n].setNoiseTurn(NOISE_TURN);
        m_particles[n].setNoiseSense(NOISE_SENSE);
    }
}

void ParticleFilter::move(const Movement &m)
{
    qDebug() << "move";
    for (int n = 0; n < m_particles.size(); n++) {
        m_particles[n].move(m);
    }
    emit particlesUpdated(m_particles);
}

void ParticleFilter::resample(const QList<Marker> &markers)
{
    qDebug() << "resample";
    size_t N = m_particles.size();
    QVector<qreal> weights(N);

    qreal maxWeight = 0.0;
    for (size_t n = 0; n < N; n++) {
        weights[n] = m_particles[n].measurementProbability(markers);
        if (weights[n] > maxWeight)
            maxWeight = weights[n];
    }

    // Resampling wheel
    QList<Robot> resampled;
    int index = int(gRandom.random() * N);
    qreal beta = 0.0;
    for (size_t n = 0; n < N; n++) {
        beta += gRandom.random() * 2.0 * maxWeight;
        while (beta > weights[index]) {
            beta -= weights[index];
            index = (index + 1) % N;
        }
        resampled.append(m_particles[index]);
    }

    m_particles = QVector<Robot>::fromList(resampled);
    emit particlesUpdated(m_particles);
    emit estimatedPosition(calculatePosition());
}

Robot ParticleFilter::calculatePosition() const
{
    qreal x = 0.0, y = 0.0, angle = 0.0;
    if (m_particles.size() > 0) {
        // orientation is tricky because it is cyclic. By normalizing
        // around the first particle we are somewhat more robust to
        // the 0=2pi problem
        qreal firstAngle = m_particles[0].angle();
        foreach (const Robot &r, m_particles) {
            x += r.position().first;
            y += r.position().second;
            angle += fmod(r.angle() - firstAngle + M_PI, 2.0 * M_PI) + (firstAngle - M_PI);
        }
        x /= m_particles.size();
        y /= m_particles.size();
        angle /= m_particles.size();
    }
    Robot r;
    r.setPosition(QPair<qreal, qreal>(x, y));
    r.setAngle(angle);
    return r;
}
