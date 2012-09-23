#include "particlefilter.h"
#include "random.h"
#include "settings.h"
#include <QDebug>

ParticleFilter::ParticleFilter(QObject *parent) :
    QObject(parent)
{
}

void ParticleFilter::init(size_t N, qreal maxPositionX, qreal maxPositionY)
{
    m_particles.resize(N);
    for (int n = 0; n < m_particles.size(); n++) {
        do {
            m_particles[n].random(maxPositionX, maxPositionY);
        } while (!m_particles[n].isEveryMarkerVisible());

        // TODO: Maybe noise could be global parameter
        m_particles[n].setNoiseForward(NOISE_FORWARD);
        m_particles[n].setNoiseTurn(NOISE_TURN);
        m_particles[n].setNoiseSense(NOISE_SENSE);
    }
}

void ParticleFilter::move(qreal distance)
{
    qDebug() << "Move particles" << distance;
    for (int n = 0; n < m_particles.size(); n++) {
        m_particles[n].move(distance);
    }
    m_bestParticle.move(distance); // TODO: Not sure if this is correct
    emit particlesUpdated(m_particles);
    emit particlesMoved();
}

void ParticleFilter::turn(qreal angle)
{
    qDebug() << "Turn particles" << angle * 180.0 / M_PI;
    for (int n = 0; n < m_particles.size(); n++) {
        m_particles[n].turn(angle);
    }
    m_bestParticle.turn(angle); // TODO: Not sure if this is correct
    emit particlesUpdated(m_particles);
    emit particlesMoved();
}

void ParticleFilter::resample(const QList<Marker> &markers)
{
    if (markers.isEmpty())
        return;

    qDebug() << "resample";
    size_t N = m_particles.size();
    QVector<qreal> weights(N);

    qreal maxWeight = 0.0;
    for (size_t n = 0; n < N; n++) {
        weights[n] = m_particles[n].measurementProbability(markers);
        if (weights[n] > maxWeight) {
            maxWeight = weights[n];
            m_bestParticle = m_particles[n];
        }
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
        // TODO: Should we pick best particle here instead?
    }

    m_particles = QVector<Robot>::fromList(resampled);
    emit particlesUpdated(m_particles);
    qDebug() << "Estimated position:" << m_bestParticle.position();
    emit estimatedPositionChanged(m_bestParticle); // TODO: Emit this before resampling wheel?
}
