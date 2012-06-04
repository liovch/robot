#include "fcamimageprovider.h"
#include <QDebug>

FCamImageProvider::FCamImageProvider(QObject *parent) :
    ImageProvider(parent),
    m_index(0)
{
}

bool FCamImageProvider::init()
{
    // Attach the lens to the sensor
    m_sensor.attach(&m_lens);

    qDebug() << "Near focus:" << m_lens.nearFocus()
             << "Far focus:" << m_lens.farFocus();

    // Explicitly power up the sensor
    if (m_sensor.initialize(0) == -1){
        qDebug() << "Error powering up the sensor.";
        return 1;
    }

    // Focus to 1D (1m) with maximal speed
    m_lens.setFocus(1.f, m_lens.maxFocusSpeed());
    qDebug() << "Changing focus";
    while (m_lens.focusChanging()) { } // Wait to be done
    qDebug() << "Focus changed";

    m_shot.exposure = 40000; // 40 ms exposure
    m_shot.gain = 1.0f;      // minimum ISO

    // Specify the output resolution and format, and allocate storage for the resulting image
    m_shot.image = FCam::Image(2304, 1296, FCam::UYVY);

    return true;
}

void FCamImageProvider::requestNextImage()
{
    // Order the sensor to capture a shot
    m_sensor.capture(m_shot);
    errorCheck();
    Q_ASSERT(m_sensor.shotsPending() == 1); // There should be exactly one shot

    // Retrieve the frame from the sensor
    FCam::Frame frame = m_sensor.getFrame();
    Q_ASSERT(frame.shot().id == m_shot.id); // This frame should be the result of the shot we made

    // This frame should be valid too
    Q_ASSERT(frame.valid());
    Q_ASSERT(frame.image().valid());

    qDebug() << "Image Format" << frame.image().type();

    // Save the frame
    char path[256];
    snprintf(path, 256, "/home/user/fcam%03d.jpg", m_index);
    FCam::saveJPEG(frame, path);
    m_index++;

    // Check that the pipeline is empty
    Q_ASSERT(m_sensor.framesPending() == 0);
    Q_ASSERT(m_sensor.shotsPending() == 0);

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
