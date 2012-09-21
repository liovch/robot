#include "markerprocessor.h"
#include "qplatformdefs.h"
#include <QImage>
#include <QDir>
#include "markerparams.h"
#include <QDebug>
#include <QPainter>

MarkerProcessor::MarkerProcessor(QObject *parent) :
    QObject(parent)
{
}

void MarkerProcessor::processMarkers(const QList<Marker> &markers)
{
//    qDebug() << "Total markers:" << markers.count();
//    foreach(Marker marker, markers) {
//        // TODO: for now just output markers to console
//        qDebug() << marker.id << marker.distance();
//    }
    m_markers = markers; // save markers to use on a map
                         // TODO: Here we have to assume that processMarkerMap always come after processMarkers :(
                         //       Does it mean we have to combine them into one object?
}
