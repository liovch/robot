#include "markerprocessor.h"
#include "qplatformdefs.h"
#include <QImage>
#include <QDir>
#include "markerparams.h"
#include <QDebug>
#include <QPainter>

MarkerProcessor::MarkerProcessor(QObject *parent) :
    QObject(parent),
    m_display(0),
    m_fileIndex(0)
{
}

void MarkerProcessor::processMarkers(const QList<Marker> &markers)
{
    qDebug() << "Total markers:" << markers.count();
    foreach(Marker marker, markers) {
        // TODO: for now just output markers to console
        qDebug() << findMarkerParams(marker.id()).name() << marker.distance();
    }
    m_markers = markers; // save markers to use on a map
                         // TODO: Here we have to assume that processMarkerMap always come after processMarkers :(
                         //       Does it mean we have to combine them into one object?
}

void MarkerProcessor::processMarkerMap(const QVector<MarkerParams::MarkerId> &markerMap, int width, int height)
{
#ifndef MEEGO_EDITION_HARMATTAN
    QImage image(width, height, QImage::Format_ARGB32);
    if (image.isNull())
        return;

    image.fill(0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (markerMap[x + y * width] != MARKER_NULL)
                image.setPixel(x, y, markerColor(markerMap[x + y * width]));
        }
    }

    QPainter painter(&image);
    QPen pen(Qt::DotLine);
    pen.setColor(qRgb(255, 255, 255));
    painter.setPen(pen);

    painter.drawLine(0, image.height() / 2, image.width(), image.height() / 2);

    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    foreach(Marker marker, m_markers) {
        pen.setColor(markerColor(marker.id()));
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawLine(marker.offset(), 0, marker.offset(), image.height());
        painter.drawLine(marker.offset() + marker.width(), 0, marker.offset() + marker.width(), image.height());
        pen.setColor(qRgb(255, 255, 255));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawLine(marker.offset(), image.height() / 2 - marker.above(), marker.offset() + marker.width(), image.height() / 2 - marker.above());
        painter.drawLine(marker.offset(), image.height() / 2 + marker.below(), marker.offset() + marker.width(), image.height() / 2 + marker.below());
    }

    QString filename;
    filename.sprintf("Image %4.d.jpg", m_fileIndex++);
    QString filepath = QFileInfo(QDir(m_dir), filename).absoluteFilePath();
    if (!image.save(filepath)) {
        qDebug() << "Unable to save image:" << filepath;
        return;
    }

    if (m_display) {
        m_display->setProperty("source", filepath);
    }
#else
    Q_UNUSED(markerMap);
    Q_UNUSED(width);
    Q_UNUSED(height);
#endif
}

QRgb MarkerProcessor::markerColor(MarkerParams::MarkerId id)
{
    switch (id) {
    case MARKER_RED: return qRgb(255, 0, 0);
    case MARKER_GREEN: return qRgb(0, 127, 0); // make it darker, clashes with white line
    case MARKER_BLUE:  return qRgb(0, 0, 255);
    }
    return qRgb(0, 0, 0);
}
