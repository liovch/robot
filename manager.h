#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include "qplatformdefs.h"
#include "imageprovider.h"
#include "imageprocessor.h"
#include "movementprovider.h"
#include "particlefilter.h"
#include "qmlapplicationviewer.h"
#include "markerprocessor.h"

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = 0);
    
    bool init();

signals:
    
public slots:
    void mouseClicked();

private:
#ifndef MEEGO_EDITION_HARMATTAN
    QmlApplicationViewer imageViewer;
    QmlApplicationViewer particleViewer;
#else
    QmlApplicationViewer m_phoneUI;
#endif

    MovementProvider movementProvider;
    ImageProvider *m_imageProvider;
    ImageProcessor imageProcessor;
    ParticleFilter particleFilter;
    MarkerProcessor markerProcessor;

    bool m_movementRequest; // used to alternate between movement and measurement updates
};

#endif // MANAGER_H
