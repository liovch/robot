#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <QObject>
#include <QtMobility/qmobilityglobal.h>
#include <QCompass>
#include <QMagnetometer>
#include "imageprovider.h"
#include "imageprocessor.h"
#include "marker.h"
#include "sensordata.h"

QTM_BEGIN_NAMESPACE
class QCompass;
class QMagnetometer;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class SensorManager : public QObject
{
    Q_OBJECT
public:
    explicit SensorManager(QObject *parent = 0);
    bool isReady() const { return m_isReady; }

signals:
    // emited when the object becomes ready to process data
    void ready();
    void dataChanged(const SensorData& data);

public slots:
    bool init();
    void requestNewData();
    void onMarkersChanged(const QList<Marker>& markers);

private slots:
    void onCompassReadingChanged();
    void onMagnetometerReadingChanged();

private:
    void signalIfReady();

    ImageProvider *m_imageProvider;
    int m_imageCaptureAttempt;
    ImageProcessor *m_imageProcessor;
    SensorData m_data;

    QCompass *m_compass;
    QCompassReading* m_compassReading;

    QMagnetometer *m_magnetometer;
    QMagnetometerReading *m_magnetometerReading;

    bool m_isCompassReady;
    bool m_isCameraReady;

    bool m_isReady;
};

#endif // SENSORMANAGER_H
