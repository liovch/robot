#include "manager.h"
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QDebug>
#include "particledisplay.h"
#include "settings.h"

Manager::Manager(QObject *parent) :
    QObject(parent),
    m_movementRequest(true)
{
}

bool Manager::init()
{
    imageViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    particleViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    imageViewer.rootContext()->setContextProperty("markerParamsModel", QVariant::fromValue(gMarkerParams));
    imageViewer.setMainQmlFile(QLatin1String("qml/robot/main.qml"));

    qmlRegisterType<ParticleDisplay>("Robot", 1, 0, "ParticleDisplay");
    particleViewer.setMainQmlFile(QLatin1String("qml/robot/particles.qml"));

    imageProvider.setDir("/Volumes/Working/liovch/data/robot/test_N9", "*.jpg");
    QObject *imageViewerQML = imageViewer.rootObject()->findChild<QObject *>("image");
    Q_ASSERT(imageViewerQML);
    markerProcessor.setImageDisplay(imageViewerQML);
    markerProcessor.setOutputDirectory("/Volumes/Working/liovch/data/robot/output");

    particleFilter.init(NUM_PARTICLES, MAX_POSITION);

    QObject::connect(&imageProvider, SIGNAL(nextImage(QImage)), &imageProcessor, SLOT(processImage(QImage)));
    QObject::connect(&imageProcessor, SIGNAL(newMarkers(QList<Marker>)), &markerProcessor, SLOT(processMarkers(QList<Marker>)));
    QObject::connect(&imageProcessor, SIGNAL(newMarkerMap(QVector<MarkerParams::MarkerId>,int,int)), &markerProcessor, SLOT(processMarkerMap(QVector<MarkerParams::MarkerId>,int,int)));
    QObject::connect(&movementProvider, SIGNAL(nextMovement(Movement)), &particleFilter, SLOT(move(Movement)));
    QObject::connect(&imageProcessor, SIGNAL(newMarkers(QList<Marker>)), &particleFilter, SLOT(resample(QList<Marker>)));

    // connect QML components
    // connect mouse
    QObject::connect(imageViewerQML, SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));
    QObject::connect(particleViewer.rootObject(), SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));

    // process image every time a parameter is changed
    foreach (QObject* obj, gMarkerParams) {
        MarkerParams *param = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(param);
        QObject::connect(param, SIGNAL(minRChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(minGChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(minBChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxRChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxGChanged()), &imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxBChanged()), &imageProcessor, SLOT(processLastImage()));
    }
    ParticleDisplay *display = particleViewer.rootObject()->findChild<ParticleDisplay *>(QString("display"));
    Q_ASSERT(display);
    QObject::connect(&particleFilter, SIGNAL(particlesUpdated(QVector<Robot>)), display, SLOT(setParticles(QVector<Robot>)));

    mouseClicked(); // request first data to show something on the screen at startup

    imageViewer.showExpanded();
    particleViewer.showExpanded();

    // set initial window positions
    QRect geometry = imageViewer.geometry();
    imageViewer.setGeometry(300, 600, geometry.width(), geometry.height());
    geometry = particleViewer.geometry();
    particleViewer.setGeometry(300, 100, geometry.width(), geometry.height());

    return true;
}

void Manager::mouseClicked()
{
    if (m_movementRequest) {
        movementProvider.requestNextMovement();
    } else {
        imageProvider.requestNextImage();
    }

    m_movementRequest = !m_movementRequest;
}
