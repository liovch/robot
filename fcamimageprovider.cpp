#include "fcamimageprovider.h"
#include "camera.h"
#include <QTimer>
#include <QDebug>

FCamImageProvider::FCamImageProvider(QObject *parent) :
    ImageProvider(parent),
    m_index(0)
{
}

bool FCamImageProvider::init()
{
    return true;
}

void FCamImageProvider::requestNextImage()
{
    // TODO: Make sure camera is ready
    //       Otherwise I'm getting "VIDIOC_STREAMON: Device or resource busy"
    qDebug() << "Taking another image soon...";
    QTimer::singleShot(300, this, SLOT(takeImage()));
}

void FCamImageProvider::takeImage()
{
    FCam::N9::Sensor sensor;
    FCam::Shot shot;
    FCam::N9::Lens lens;

    // Attach the lens to the sensor
    sensor.attach(&lens);

    // Explicitly power up the sensor
    if (sensor.initialize(0) == -1){
        qDebug() << "Error powering up the sensor.";
        return;
    }

    // Focus to 1D (1m) with maximal speed
    lens.setFocus(1.f, lens.maxFocusSpeed());
    while (lens.focusChanging()) { } // Wait to be done

    shot.exposure = 40000; // 40 ms exposure
    shot.gain = 1.0f;      // minimum ISO

    // Specify the output resolution and format, and allocate storage for the resulting image
    shot.image = FCam::Image(gCamera.width(), gCamera.height(), FCam::UYVY);

    // Order the sensor to capture a shot
    sensor.capture(shot);
    errorCheck();
    Q_ASSERT(sensor.shotsPending() == 1); // There should be exactly one shot

    // Retrieve the frame from the sensor
    FCam::Frame frame = sensor.getFrame();
    Q_ASSERT(frame.shot().id == shot.id); // This frame should be the result of the shot we made

    // This frame should be valid too
    Q_ASSERT(frame.valid());
    Q_ASSERT(frame.image().valid());

    // TODO: Convert from FCam::Frame to QImage directly
    // Save the frame with maximum JPG quality
    char path[256];
    snprintf(path, 256, "/home/user/fcam%03d.jpg", m_index);
    FCam::saveJPEG(frame, path, 100);
    qDebug() << "Image saved to" << path;
    m_index++;

    // Check that the pipeline is empty
    Q_ASSERT(sensor.framesPending() == 0);
    Q_ASSERT(sensor.shotsPending() == 0);

    QImage image(path);
    if (image.isNull()) {
        qDebug() << "Failed to load image saved from camera:" << path;
        return;
    }

    emit nextImage(image);
}

void FCamImageProvider::errorCheck()
{
    // Make sure FCam is running properly by looking for DriverError
    FCam::Event e;
    while (FCam::getNextEvent(&e, FCam::Event::Error)) {
        printf("Error: %s\n", e.description.c_str());
        if (e.data == FCam::Event::DriverMissingError) {
            qDebug() << "FCam can't find its driver. Did you install "
                        "fcam-drivers on your platform, and reboot the device "
                        "after installation?";
        }
        if (e.data == FCam::Event::DriverLockedError) {
            qDebug() << "Another FCam program appears to be running "
                        "already. Only one can run at a time.";
        }
    }
}
