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

    double offset() const { return m_offset; }
    double width() const { return m_width; }
    double height() const { return m_height; }
    MarkerParams::MarkerId id() const { return m_id; }
    double above() const { return m_above; }
    double below() const { return m_below; }

    // computes distance from camera to this marker based on the height of this marker
    double distance() const;

    void setOffset(double offset) { m_offset = offset; }
    void setWidth(double width) { m_width = width; }
    void setHeight(double height) { m_height = height; }
    void setId(MarkerParams::MarkerId id) { m_id = id; }
    void setAbove(double above) { m_above = above; }
    void setBelow(double below) { m_below = below; }

    Marker& operator =(const Marker& marker);

signals:

public slots:

private:
    double m_offset; // horizontal offset from left edge of the image in pixels
    double m_width;  // width in pixels
    double m_height; // height in pixels
    MarkerParams::MarkerId m_id;

    // These are non-essential parameters
    double m_above; // amount of pixels above the middle line
    double m_below; // amount of pixels below the middle line
};

#endif // MARKER_H
