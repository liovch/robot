#include "manager.h"
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QDebug>
#include "settings.h"
#ifndef MEEGO_EDITION_HARMATTAN
#include "particledisplay.h"
#include "folderimageprovider.h"
#else
#include "cameraimageprovider.h"
#endif

Manager::Manager(QObject *parent) :
    QObject(parent),
    m_imageProvider(0),
    m_movementRequest(true)
{
}

bool Manager::init()
{
#ifndef MEEGO_EDITION_HARMATTAN
    imageViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    particleViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    imageViewer.rootContext()->setContextProperty("markerParamsModel", QVariant::fromValue(gMarkerParams));
    imageViewer.setMainQmlFile(QLatin1String("qml/robot/imageviewer.qml"));

    qmlRegisterType<ParticleDisplay>("Robot", 1, 0, "ParticleDisplay");
    particleViewer.setMainQmlFile(QLatin1String("qml/robot/particleviewer.qml"));

    FolderImageProvider *imageProvider = new FolderImageProvider(this);
    imageProvider->setDir("../../data/robot/test", "*.jpg");

    QObject *imageViewerQML = imageViewer.rootObject()->findChild<QObject *>("image");
    Q_ASSERT(imageViewerQML);
    markerProcessor.setImageDisplay(imageViewerQML);
    markerProcessor.setOutputDirectory("../../data/robot/output");
#else
    m_phoneUI.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    m_phoneUI.setMainQmlFile("qml/robot/phone.qml");

    CameraImageProvider *imageProvider = new CameraImageProvider(this);
    QObject *camera = m_phoneUI.rootObject()->findChild<QObject *>(QString("camera"));
    Q_ASSERT(camera);
    imageProvider->init(camera);
#endif

    m_imageProvider = imageProvider;

    particleFilter.init(NUM_PARTICLES, MAX_POSITION);

    QObject::connect(m_imageProvider, SIGNAL(nextImage(QImage)), &imageProcessor, SLOT(processImage(QImage)));
    QObject::connect(&imageProcessor, SIGNAL(newMarkers(QList<Marker>)), &markerProcessor, SLOT(processMarkers(QList<Marker>)));
    QObject::connect(&imageProcessor, SIGNAL(newMarkerMap(QVector<MarkerParams::MarkerId>,int,int)), &markerProcessor, SLOT(processMarkerMap(QVector<MarkerParams::MarkerId>,int,int)));
    QObject::connect(&movementProvider, SIGNAL(nextMovement(Movement)), &particleFilter, SLOT(move(Movement)));
    QObject::connect(&imageProcessor, SIGNAL(newMarkers(QList<Marker>)), &particleFilter, SLOT(resample(QList<Marker>)));

#ifndef MEEGO_EDITION_HARMATTAN
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
    particleViewer.setGeometry(300, 15, geometry.width(), geometry.height());
#else
    QObject::connect(m_phoneUI.rootObject(), SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));
    mouseClicked(); // request first data to show something on the screen at startup
    m_phoneUI.showExpanded();
#endif
    return true;
}

void Manager::mouseClicked()
{
    if (m_movementRequest) {
        movementProvider.requestNextMovement();
    } else {
        m_imageProvider->requestNextImage();
    }

    m_movementRequest = !m_movementRequest;
}
