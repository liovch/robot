#include "sensormanager.h"
#include "artoolkitimageprocessor.h"
#include "fcamimageprovider.h"
#include "helpers.h"
#include <qmath.h>
#include <QDebug>
#include <QTimer>

#define MAX_IMAGE_CAPTURE_ATTEMPTS 5

SensorManager::SensorManager(QObject *parent) :
    QObject(parent),
    m_imageProvider(0),
    m_imageCaptureAttempt(0),
    m_imageProcessor(0),
    m_magnetometer(0),
    m_magnetometerReading(0),
    m_isMagnetometerReady(false),
    m_isCameraReady(false),
    m_isReady(false)
{
}

bool SensorManager::init()
{
    FCamImageProvider *imageProvider = new FCamImageProvider(this);
    if (!imageProvider->init()) {
        qWarning() << "Failed to initialize camera";
        return false;
    }

    m_magnetometer = new QMagnetometer(this);
    m_magnetometer->setProperty("returnGeoValues", QVariant(true));
    if (!m_magnetometer->start()) {
        qWarning() << "Failed to start magnetometer sensor.";
        return false;
    }

    m_imageProvider = imageProvider;
    m_imageProcessor = new ARToolkitImageProcessor(this);
    m_isCameraReady = true; // TODO: Add an actual signal?

    QObject::connect(m_imageProvider, SIGNAL(nextImage(QImage)), m_imageProcessor, SLOT(processImage(QImage)));
    QObject::connect(m_imageProcessor, SIGNAL(markersChanged(QList<Marker>)), this, SLOT(onMarkersChanged(QList<Marker>)));

    QObject::connect(m_magnetometer, SIGNAL(readingChanged()), this, SLOT(onMagnetometerReadingChanged()));
    return true;
}

void SensorManager::onMarkersChanged(const QList<Marker> &markers)
{
    qDebug() << "onMarkersChanged:" << markers.isEmpty();
    m_data.m_markers = markers;
    if (m_data.m_markers.isEmpty() && m_imageCaptureAttempt < MAX_IMAGE_CAPTURE_ATTEMPTS) {
        qDebug() << "Requesting another image:" << m_imageCaptureAttempt << "of" << MAX_IMAGE_CAPTURE_ATTEMPTS;
        m_imageCaptureAttempt++;
        m_imageProvider->requestNextImage();
    } else {
        Q_ASSERT(m_compassReading->calibrationLevel() >= 1.0);
        m_data.m_angle = magnetometerToAngle(m_magnetometerReading->x(), m_magnetometerReading->y());
        emit dataChanged(m_data);
    }
}

void SensorManager::onMagnetometerReadingChanged()
{
    if (!m_isMagnetometerReady) {
        m_magnetometerReading = m_magnetometer->reading();
        qreal calibrationLevel = m_magnetometerReading->calibrationLevel();

        if (calibrationLevel < 1.0) {
            qDebug() << "Magnetometer calibration level is too low:" << calibrationLevel;
        } else {
            m_isMagnetometerReady = true;
            qDebug() << "Magnetometer is ready";
            signalIfReady();
        }
    }
//    else {
//        qDebug() << "Magnetometer reading: " << magnetometerToAngle(m_magnetometerReading->x(), m_magnetometerReading->y()) * 180.0 / M_PI;
//    }
}

void SensorManager::signalIfReady()
{
    if (m_isReady)
        return;

    if (m_isMagnetometerReady && m_isCameraReady) {
        qDebug() << "SensorManager is ready.";
        m_isReady = true;
        emit ready();
    }
}

void SensorManager::requestNewData()
{
    if (!m_isReady) {
        qDebug() << "SensorManager is not ready.";
        return;
    }

    m_data.m_markers.clear();
    m_imageCaptureAttempt = 0;
    m_imageProvider->requestNextImage();
}
