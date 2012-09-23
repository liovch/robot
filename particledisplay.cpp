#include "particledisplay.h"
#include <QDateTime>
#include <QPainter>
#include <qmath.h>
#include "settings.h"
#include <QDebug>

ParticleDisplay::ParticleDisplay(QObject *parent) :
    QObject(parent),
    m_particleSize(2.0), // default particle size in pixels
    m_directionVectorSize(5.0), // default direction vector size in pixels
    m_markerSize(7.0) // default marker size in pixels
{
}

void ParticleDisplay::setParticles(const QVector<Robot> &particles)
{
    m_particles = particles;
    saveImage();
}

void ParticleDisplay::saveImage()
{
    QImage image(640, 320, QImage::Format_RGB32);
    QPainter painter(&image);
    painter.fillRect(image.rect(), QColor(Qt::white));

//    painter->scale(1.0, -1.0);
//    painter->translate(1.0, -boundingRect().height());

    // draw markers
    foreach (MarkerParams params, gMarkerParams) {
        //TODO: painter->setBrush(QBrush(QColor(params->name())));
        QPointF p((params.x() / GRID_MAP_WIDTH) * image.width(), (params.y() / GRID_MAP_HEIGHT) * image.height());
        painter.drawRect(QRectF(p.x() - (m_markerSize / 2.0), p.y() - (m_markerSize / 2.0), m_markerSize, m_markerSize));
    }

    painter.setBrush(QBrush(QColor("black")));
    foreach (Robot r, m_particles) {
        QPointF p1((r.position().first / GRID_MAP_WIDTH) * image.width(), (r.position().second / GRID_MAP_HEIGHT) * image.height());
        painter.drawRect(QRectF(p1.x() - (m_particleSize / 2.0), p1.y() - (m_particleSize / 2.0), m_particleSize, m_particleSize));
        QPointF p2(p1.x() + qCos(r.angle()) * m_directionVectorSize, p1.y() + qSin(r.angle()) * m_directionVectorSize);
        painter.drawLine(p1, p2);
    }

    QString filePath = PLAN_PATH + QDateTime::currentDateTime().toString("'ParticleFilter: 'yyyy.MM.dd - hh_mm_ss_zzz") + ".png";
    image.save(filePath);
    qDebug() << "Saved particle filter state to:" << filePath;
}
