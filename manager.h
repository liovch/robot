#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include "qplatformdefs.h"
#include "imageprovider.h"
#include "imageprocessor.h"
#include "movementprovider.h"
#include "particlefilter.h"
#ifndef MEEGO_EDITION_HARMATTAN
#include "qmlapplicationviewer.h"
#include "markerprocessor.h"
#endif

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
#endif

    MovementProvider movementProvider;
    ImageProvider *m_imageProvider;
    ImageProcessor imageProcessor;
    ParticleFilter particleFilter;
#ifndef MEEGO_EDITION_HARMATTAN
    MarkerProcessor markerProcessor;
#endif

    bool m_movementRequest; // used to alternate between movement and measurement updates
};

#endif // MANAGER_H
