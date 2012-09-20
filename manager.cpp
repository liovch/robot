#include "manager.h"
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QDebug>
#include "settings.h"
#include "artoolkitimageprocessor.h"
#ifndef MEEGO_EDITION_HARMATTAN
    #include "particledisplay.h"
    #include "folderimageprovider.h"
    #include "fakemotionproxy.h"
#else
    #include "fcamimageprovider.h"
    #include "realmotionproxy.h"
#endif

Manager::Manager(QObject *parent) :
    QObject(parent),
    m_motionProxy(0),
    m_imageProvider(0),
    m_imageProcessor(0)
{
}

bool Manager::init()
{
#ifndef MEEGO_EDITION_HARMATTAN
    imageViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    particleViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    initMarkerParams();
    imageViewer.setMainQmlFile(QLatin1String("qml/robot/imageviewer.qml"));
    QObject *imageViewerQML = imageViewer.rootObject()->findChild<QObject *>("image");
    Q_ASSERT(imageViewerQML);

    qmlRegisterType<ParticleDisplay>("Robot", 1, 0, "ParticleDisplay");
    particleViewer.setMainQmlFile(QLatin1String("qml/robot/particleviewer.qml"));

    FolderImageProvider *imageProvider = new FolderImageProvider(this);
    imageProvider->setDir("../../data/robot/artoolkit", "*.jpg");
    imageProvider->setImageDisplay(imageViewerQML);

    m_motionProxy = new FakeMotionProxy(this);
#else
    m_phoneUI.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    m_phoneUI.setMainQmlFile("qml/robot/camera.qml");

    FCamImageProvider *imageProvider = new FCamImageProvider(this);
    if (!imageProvider->init()) {
        qDebug() << "Failed to initialize camera";
        return false;
    }

    m_motionProxy = new RealMotionProxy(this);
#endif // !MEEGO_EDITION_HARMATTAN

    m_imageProvider = imageProvider;
    m_imageProcessor = new ARToolkitImageProcessor(this);

    particleFilter.init(NUM_PARTICLES, GRID_MAP_WIDTH, GRID_MAP_HEIGHT);
    if (!m_motionPlanner.loadGridMap(GRID_MAP_PATH)) {
        qDebug() << "Failed to load motion planner map.";
    }
    m_motionPlanner.setGoal(100, 50);

    QObject::connect(m_imageProvider, SIGNAL(nextImage(QImage)), m_imageProcessor, SLOT(processImage(QImage)));
    QObject::connect(m_imageProcessor, SIGNAL(imageProcessed(QList<Marker>)), &particleFilter, SLOT(resample(QList<Marker>)));
    QObject::connect(&particleFilter, SIGNAL(estimatedPosition(Robot)), &m_motionPlanner, SLOT(requestNextUpdate(Robot)));
    QObject::connect(&m_motionPlanner, SIGNAL(motionUpdate(Movement)), m_motionProxy, SLOT(motionUpdate(Movement)));
    QObject::connect(m_motionProxy, SIGNAL(finishedMotionUpdate(Movement)), &particleFilter, SLOT(move(Movement)));
    // TODO: Connect failedMotionUpdate signal to motion planner

    // TODO: MarkerProcessor is only used to print markers for debugging
    QObject::connect(m_imageProcessor, SIGNAL(imageProcessed(QList<Marker>)), &markerProcessor, SLOT(processMarkers(QList<Marker>)));

#ifndef MEEGO_EDITION_HARMATTAN
    // connect QML components
    // connect mouse
    QObject::connect(imageViewerQML, SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));
    QObject::connect(particleViewer.rootObject(), SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));

    ParticleDisplay *display = particleViewer.rootObject()->findChild<ParticleDisplay *>(QString("display"));
    Q_ASSERT(display);
    QObject::connect(&particleFilter, SIGNAL(particlesUpdated(QVector<Robot>)), display, SLOT(setParticles(QVector<Robot>)));

    m_imageProvider->requestNextImage(); // request first data to show something on the screen at startup

    imageViewer.showExpanded();
    particleViewer.showExpanded();

    // set initial window positions
    QRect geometry = imageViewer.geometry();
    imageViewer.setGeometry(300, 600, geometry.width(), geometry.height());
    geometry = particleViewer.geometry();
    particleViewer.setGeometry(300, 15, GRID_MAP_WIDTH * 10.0 * 5.0, GRID_MAP_HEIGHT * 10.0 * 5.0);
#else
    QObject::connect(m_phoneUI.rootObject(), SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));
    m_imageProvider->requestNextImage(); // request first data to show something on the screen at startup
    m_phoneUI.showExpanded();
#endif
    return true;
}

void Manager::mouseClicked()
{
    // TODO: Probably remove this
    m_imageProvider->requestNextImage();
}
