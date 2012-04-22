#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include "marker.h"

class Camera : public QObject
{
    Q_OBJECT
public:
    explicit Camera(QObject *parent = 0);
    
    Q_PROPERTY(int scale READ scale WRITE setScale NOTIFY scaleChanged);

    // calculates distance from camera to marker
    double distance(const Marker& marker) const;

    // How much the camera image is scaled.
    // Default is 0: 2^0 = 1
    int scale() const { return m_scalePowerOfTwo; }
    void setScale(int powerOfTwo);

    qreal angleOfView() const;

signals:
    void scaleChanged();

public slots:
    
private:
    int m_scalePowerOfTwo;
};

extern Camera gCamera;

#endif // CAMERA_H
