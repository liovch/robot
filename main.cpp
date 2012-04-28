#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include "qmlapplicationviewer.h"
#include "imageprocessor.h"
#include "imageprovider.h"
#include "markerprocessor.h"
#include "movementprovider.h"
#include "particledisplay.h"
#include "particlefilter.h"
#include "settings.h"
#include <QDebug>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    // Setup image provider, image processor, and marker processor
    ImageProvider imageProvider;
    imageProvider.setDir("/Volumes/Working/liovch/data/robot/test_N9", "*.jpg");

    ImageProcessor imageProcessor;
    QObject::connect(&imageProvider, SIGNAL(nextImage(QImage)), &imageProcessor, SLOT(processImage(QImage)));

    MarkerProcessor markerProcessor;
    QObject::connect(&imageProcessor, SIGNAL(newMarkers(QList<Marker>)), &markerProcessor, SLOT(processMarkers(QList<Marker>)));
    QObject::connect(&imageProcessor, SIGNAL(newMarkerMap(QVector<MarkerParams::MarkerId>,int,int)), &markerProcessor, SLOT(processMarkerMap(QVector<MarkerParams::MarkerId>,int,int)));


    // Setup image viewer
    QmlApplicationViewer imageViewer;
    imageViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    // setup the model
    imageViewer.rootContext()->setContextProperty("markerParamsModel", QVariant::fromValue(gMarkerParams));

    imageViewer.setMainQmlFile(QLatin1String("qml/robot/main.qml"));

    QDeclarativeItem* object = imageViewer.rootObject()->findChild<QDeclarativeItem *>(QString("imageRect"));
    QObject *qmlImage = object->findChild<QObject *>(QString("image"));
    if (!qmlImage) {
        qDebug() << "QML image not found";
        return -1;
    }

    markerProcessor.setImageDisplay(qmlImage);
    markerProcessor.setOutputDirectory("/Volumes/Working/liovch/data/robot/output");

    // connect signals
    // switch to next image when image control is clicked
    QObject::connect(object, SIGNAL(qmlClicked()), &imageProvider, SLOT(requestNextImage()));
    // process image every time a parameter is changed
    foreach (QObject* obj, gMarkerParams) {
        MarkerParams *param = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(param);
        QObject::connect(param, SIGNAL(minRChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(minGChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(minBChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxRChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxGChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxBChanged()), &imageProcessor, SLOT(processLastImage()));    }


    // Setup movement provider and particle filter
    MovementProvider movementProvider;
    QObject::connect(object, SIGNAL(qmlClicked()), &movementProvider, SLOT(requestNextMovement()));

    ParticleFilter particleFilter;
    particleFilter.init(NUM_PARTICLES, MAX_POSITION);
    QObject::connect(&movementProvider, SIGNAL(nextMovement(Movement)), &particleFilter, SLOT(move(Movement)));
    QObject::connect(&imageProcessor, SIGNAL(newMarkers(QList<Marker>)), &particleFilter, SLOT(resample(QList<Marker>)));

    // Setup particle viewer
    QmlApplicationViewer particleViewer;
    particleViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    qmlRegisterType<ParticleDisplay>("Robot", 1, 0, "ParticleDisplay");
    particleViewer.setMainQmlFile(QLatin1String("qml/robot/particles.qml"));

    ParticleDisplay *display = particleViewer.rootObject()->findChild<ParticleDisplay *>(QString("display"));
    if (!display) {
        qDebug() << "Particle display not found";
        return -1;
    }
    QObject::connect(&particleFilter, SIGNAL(particlesUpdated(QVector<Robot>)), display, SLOT(setParticles(QVector<Robot>)));

    // Start the sytem
    movementProvider.requestNextMovement(); // request first movement
    imageProvider.requestNextImage(); // request first image

    imageViewer.showExpanded();
    particleViewer.showExpanded();
    return app->exec();
}
