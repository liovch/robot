#ifndef MARKERPARAMS_H
#define MARKERPARAMS_H

#include <QObject>
#include <QMap>

#define MARKER_NULL -1

// Each instance of this class describes a unique marker
class MarkerParams : public QObject
{
    Q_OBJECT
public:
    explicit MarkerParams(QObject *parent = 0);
    MarkerParams(const MarkerParams& params);

    void setId(int id) { m_id = id; }
    void setPosition(qreal x, qreal y, qreal z) { m_x = x; m_y = y; m_z = z; }

    static qreal size(); // returns the size of a square marker in m

    int id() const { return m_id; }
    qreal x() const { return m_x; }
    qreal y() const { return m_y; }
    qreal z() const { return m_z; }

    MarkerParams& operator=(const MarkerParams& params);

private:
    int m_id; // unique marker id (-1 means no marker)
    qreal m_x, m_y, m_z; // absolute position of the marker (in mm)
};

extern QMap<int, MarkerParams> gMarkerParams;
void initMarkerParams();

#endif // MARKERPARAMS_H
