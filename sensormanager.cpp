#include "sensormanager.h"
#include "artoolkitimageprocessor.h"
#include "fcamimageprovider.h"
#include <QDebug>
#include <QTimer>

#define MAX_IMAGE_CAPTURE_ATTEMPTS 5

SensorManager::SensorManager(QObject *parent) :
    QObject(parent),
    m_imageProvider(0),
    m_imageCaptureAttempt(0),
    m_imageProcessor(0),
    m_compass(0),
    m_compassReading(0),
    m_isCompassReady(false),
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

    m_compass = new QCompass(this);
    m_compass->setDataRate(10);
    if (!m_compass->start()) {
        qWarning() << "Failed to start compass sensor.";
        return false;
    }

    m_imageProvider = imageProvider;
    m_imageProcessor = new ARToolkitImageProcessor(this);

    QObject::connect(m_imageProvider, SIGNAL(nextImage(QImage)), m_imageProcessor, SLOT(processImage(QImage)));
    QObject::connect(m_imageProcessor, SIGNAL(imageProcessed(QList<Marker>)), this, SLOT(onMarkersChanged(QList<Marker>)));

    QObject::connect(m_compass, SIGNAL(readingChanged()), this, SLOT(onCompassReadingChanged()));
    return true;
}

void SensorManager::onMarkersChanged(const QList<Marker> &markers)
{
    m_data.m_markers = markers;
    if (m_data.m_markers.isEmpty() && m_imageCaptureAttempt < MAX_IMAGE_CAPTURE_ATTEMPTS) {
        m_imageCaptureAttempt++;
        m_imageProvider->requestNextImage();
    } else {
        Q_ASSERT(m_compassReading->calibrationLevel() >= 1.0);
        m_data.m_azimuth = m_compassReading->azimuth();
        emit dataChanged(m_data);
    }
}

void SensorManager::onCompassReadingChanged()
{
    if (!m_isCompassReady) {
        m_compassReading = m_compass->reading();
        qreal calibrationLevel = m_compassReading->calibrationLevel();
        if (calibrationLevel < 1.0) {
            qDebug() << "Compass calibration level is too low:" << calibrationLevel;
        } else if (!m_isCompassReady) {
            m_isCompassReady = true;
            qDebug() << "Compass is ready";
            signalIfReady();
        }
    }
    qDebug() << "Azimuth: " << m_compassReading->azimuth() << m_compassReading->calibrationLevel();
}

void SensorManager::signalIfReady()
{
    if (m_isReady)
        return;

    if (m_isCompassReady && m_isCameraReady) {
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
