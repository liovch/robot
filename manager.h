#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include "qmlapplicationviewer.h"
#include "imageprovider.h"
#include "imageprocessor.h"
#include "markerprocessor.h"
#include "movementprovider.h"
#include "particlefilter.h"

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
    QmlApplicationViewer imageViewer;
    QmlApplicationViewer particleViewer;

    MovementProvider movementProvider;
    FolderImageProvider imageProvider;
    ImageProcessor imageProcessor;
    MarkerProcessor markerProcessor;
    ParticleFilter particleFilter;

    bool m_movementRequest; // used to alternate between movement and measurement updates
};

#endif // MANAGER_H
