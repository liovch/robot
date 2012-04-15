#ifndef MARKERPARAMS_H
#define MARKERPARAMS_H

#include <QObject>
#include <QColor>
#include <QMap>
#include <QPair>

class MarkerParams : public QObject
{
    Q_OBJECT
public:
    explicit MarkerParams(QObject *parent = 0);
    MarkerParams(const MarkerParams& params);

    #define MARKER_NULL  0
    #define MARKER_RED   1
    #define MARKER_GREEN 2
    #define MARKER_BLUE  3
    typedef uint MarkerId;

    void setHeight(double height) { m_height = height; }
    void setRange(const QPair<QRgb, QRgb>& range) { m_range = range; }
    void setId(MarkerId id) { m_id = id; }

    double height() const { return m_height; }
    QPair<QRgb, QRgb> range() const { return m_range; }
    MarkerId id() const { return m_id; }

    MarkerParams& operator=(const MarkerParams& params);

signals:

public slots:

private:
    double m_height; // pgysical height of the marker in meters
    QPair<QRgb, QRgb> m_range; // color range for this marker
    MarkerId m_id; // unique marker id (0 means no marker)
};

extern QMap<MarkerParams::MarkerId, MarkerParams> gMarkerParams;
void initMarkerParams();

#endif // MARKERPARAMS_H
