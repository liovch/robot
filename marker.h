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

    qreal offset() const { return m_offset; }
    qreal width() const { return m_width; }
    qreal height() const { return m_height; }
    MarkerParams::MarkerId id() const { return m_id; }
    qreal above() const { return m_above; }
    qreal below() const { return m_below; }

    // computes distance from camera to this marker based on the height of this marker
    qreal distance() const;

    void setOffset(qreal offset) { m_offset = offset; }
    void setWidth(qreal width) { m_width = width; }
    void setHeight(qreal height) { m_height = height; }
    void setId(MarkerParams::MarkerId id) { m_id = id; }
    void setAbove(qreal above) { m_above = above; }
    void setBelow(qreal below) { m_below = below; }

    Marker& operator =(const Marker& marker);

signals:

public slots:

private:
    qreal m_offset; // horizontal offset from left edge of the image in pixels
    qreal m_width;  // width in pixels
    qreal m_height; // height in pixels
    MarkerParams::MarkerId m_id;

    // These are non-essential parameters
    qreal m_above; // amount of pixels above the middle line
    qreal m_below; // amount of pixels below the middle line
};

#endif // MARKER_H
