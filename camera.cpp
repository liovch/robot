#include "camera.h"
#include "markerparams.h"
#include <qmath.h>

#define CAMERA_WIDTH   2304
#define CAMERA_HEIGHT  1296
#ifdef USE_AR_TOOLKIT
#define CAMERA_SCALE   0        // 2 << CAMERA_SCALE
#else
#define CAMERA_SCALE   1
#endif
#define PIXEL_HEIGHT   1.478E-6 // physical size of a pixel on the camera sensor (in m)
#define FOCAL_DISTANCE 3.77E-3  // focal distance of the camera (in m)
#define ANGLE_OF_VIEW  70.0     // camera angle of view (in degrees)

Camera gCamera;

Camera::Camera(QObject *parent) :
    QObject(parent),
    m_scalePowerOfTwo(CAMERA_SCALE)
{
}

int Camera::width() const
{
    return CAMERA_WIDTH;
}

int Camera::height() const
{
    return CAMERA_HEIGHT;
}

void Camera::setScale(int powerOfTwo)
{
    if (m_scalePowerOfTwo != powerOfTwo) {
        m_scalePowerOfTwo = powerOfTwo;
        emit scaleChanged();
    }
}

qreal Camera::angleOfView() const
{
    return (ANGLE_OF_VIEW * M_PI) / 180.0;
}
