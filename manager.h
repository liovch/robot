#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include "qplatformdefs.h"
#include "imageprovider.h"
#include "imageprocessor.h"
#include "motionplanner.h"
#include "particlefilter.h"
#include "qmlapplicationviewer.h"
#include "markerprocessor.h"
#include "motionproxy.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    
    bool init();

signals:
    
public slots:
    void noMarkersFound();
    void mouseClicked();

private:
#ifndef MEEGO_EDITION_HARMATTAN
    QmlApplicationViewer imageViewer;
    QmlApplicationViewer particleViewer;
#else
    QmlApplicationViewer m_phoneUI;
#endif

    MotionProxy *m_motionProxy;
    MotionPlanner m_motionPlanner;
    ImageProvider *m_imageProvider;
    ImageProcessor *m_imageProcessor;
    ParticleFilter particleFilter;
    MarkerProcessor markerProcessor;

    int m_imageCaptureAttempt;
};

#endif // MANAGER_H
