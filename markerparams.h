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

    // QML data model
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString minR READ minR WRITE setMinR NOTIFY minRChanged);
    Q_PROPERTY(QString minG READ minG WRITE setMinG NOTIFY minGChanged);
    Q_PROPERTY(QString minB READ minB WRITE setMinB NOTIFY minBChanged);
    Q_PROPERTY(QString maxR READ maxR WRITE setMaxR NOTIFY maxRChanged);
    Q_PROPERTY(QString maxG READ maxG WRITE setMaxG NOTIFY maxGChanged);
    Q_PROPERTY(QString maxB READ maxB WRITE setMaxB NOTIFY maxBChanged);

    QString name() const { return m_name; }
    void setName(const QString& name);
    QString minR() const { return QString::number(qRed(m_range.first)); }
    QString minG() const { return QString::number(qGreen(m_range.first)); }
    QString minB() const { return QString::number(qBlue(m_range.first)); }
    void setMinR(const QString& minR);
    void setMinG(const QString& minG);
    void setMinB(const QString& minB);
    QString maxR() const { return QString::number(qRed(m_range.second)); }
    QString maxG() const { return QString::number(qGreen(m_range.second)); }
    QString maxB() const { return QString::number(qBlue(m_range.second)); }
    void setMaxR(const QString& maxR);
    void setMaxG(const QString& maxG);
    void setMaxB(const QString& maxB);

    #define MARKER_NULL  0
    #define MARKER_RED   1
    #define MARKER_GREEN 2
    #define MARKER_BLUE  3
    typedef uint MarkerId;

    void setHeight(double height) { m_height = height; }
    void setRange(const QPair<QRgb, QRgb>& range);
    void setId(MarkerId id);

    double height() const { return m_height; }
    QPair<QRgb, QRgb> range() const { return m_range; }
    MarkerId id() const { return m_id; }

    MarkerParams& operator=(const MarkerParams& params);

signals:
    void nameChanged();
    void minRChanged();
    void minGChanged();
    void minBChanged();
    void maxRChanged();
    void maxGChanged();
    void maxBChanged();

public slots:

private:
    double m_height; // pgysical height of the marker in meters
    QPair<QRgb, QRgb> m_range; // color range for this marker
    // Note: Cannot use unique name as a marker id because we're building 2D map of markers in image processor
    // TODO: Maybe we could still use 2D map of QStrings with empty string for pixels where no markers found
    MarkerId m_id; // unique marker id (0 means no marker)
    QString m_name; // marker name (optional)
};

extern QList<QObject*> gMarkerParams;
void initMarkerParams();
MarkerParams findMarkerParams(MarkerParams::MarkerId id);

#endif // MARKERPARAMS_H
