#include "camera.h"
#include "markerparams.h"

#define CAMERA_SCALE   1        // 2 << CAMERA_SCALE
#define PIXEL_HEIGHT   1.478E-6 // physical size of a pixel on the camera sensor (in m)
#define FOCAL_DISTANCE 3.77E-3  // focal distance of the camera (in m)

Camera gCamera;

Camera::Camera(QObject *parent) :
    QObject(parent),
    m_scalePowerOfTwo(CAMERA_SCALE)
{
}

double Camera::distance(const Marker &marker) const
{
    double scaledPixelHeight = PIXEL_HEIGHT * double(2 << m_scalePowerOfTwo);
    return findMarkerParams(marker.id()).height() * FOCAL_DISTANCE / (marker.height() * scaledPixelHeight);
}

void Camera::setScale(int powerOfTwo)
{
    if (m_scalePowerOfTwo != powerOfTwo) {
        m_scalePowerOfTwo = powerOfTwo;
        emit scaleChanged();
    }
}
