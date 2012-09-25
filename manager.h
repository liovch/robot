#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include "qplatformdefs.h"
#include "motionplanner.h"
#include "particledisplay.h"
#include "particlefilter.h"
#include "qmlapplicationviewer.h"
//#include "markerprocessor.h"
#include "motionproxy.h"
#include "sensormanager.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    
    bool init();
    bool isReady() const { return m_isReady; }

signals:
    void ready();
    
public slots:
    void mouseClicked();

private slots:
    void checkIfReady();

private:
#ifndef MEEGO_EDITION_HARMATTAN
    QmlApplicationViewer imageViewer;
    QmlApplicationViewer particleViewer;
#else
    QmlApplicationViewer m_phoneUI;
#endif

    SensorManager *m_sensorManager;
    MotionProxy *m_motionProxy;
    MotionPlanner m_motionPlanner;
    ParticleDisplay m_particleDisplay;
    ParticleFilter particleFilter;
//    MarkerProcessor markerProcessor;

    bool m_isReady;
};

#endif // MANAGER_H
