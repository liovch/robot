#ifndef MARKER_H
#define MARKER_H

#include <QObject>
#include "markerparams.h"

class Marker : public QObject
{
    Q_OBJECT
public:
    explicit Marker(QObject *parent = 0);
    Marker(const Marker& marker);
    //bool operator=(const Marker& marker);
    bool operator==(const Marker& marker);

    // computes distance from camera to this marker
    qreal distance() const;

    int id;
    float confidence;
    float pos[2];
    float line[4][3];
    float vertex[4][2];
    float modelView[16];

signals:
public slots:
};

#endif // MARKER_H
