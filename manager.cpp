#include "manager.h"
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QDebug>
#include "settings.h"
#ifndef MEEGO_EDITION_HARMATTAN
    #include "folderimageprovider.h"
    #include "fakemotionproxy.h"
#else
    #include "realmotionproxy.h"
#endif

Manager::Manager(QObject *parent) :
    QObject(parent),
    m_motionProxy(0),
    m_isReady(false)
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

    m_motionProxy = new RealMotionProxy(this);
#endif // !MEEGO_EDITION_HARMATTAN

    m_sensorManager = new SensorManager(this);
    if (!m_sensorManager->init()) {
        qDebug() << "Failed to init sensor manager";
        return false;
    }

    particleFilter.init(NUM_PARTICLES, GRID_MAP_WIDTH, GRID_MAP_HEIGHT);
    if (!m_motionPlanner.loadGridMap(GRID_MAP_PATH)) {
        qDebug() << "Failed to load motion planner map.";
        return false;
    }
    m_motionPlanner.setGoal(10, 10); // TODO: Shouldn't this be in meters?

    QObject::connect(m_sensorManager, SIGNAL(dataChanged(SensorData)), &particleFilter, SLOT(resample(SensorData)));
    QObject::connect(&particleFilter, SIGNAL(estimatedPositionChanged(Robot)), &m_motionPlanner, SLOT(requestNextUpdate(Robot)));
    QObject::connect(&m_motionPlanner, SIGNAL(moveRequest(qreal)), m_motionProxy, SLOT(moveRequest(qreal)));
    QObject::connect(&m_motionPlanner, SIGNAL(turnRequest(qreal)), m_motionProxy, SLOT(turnRequest(qreal)));
    QObject::connect(m_motionProxy, SIGNAL(moveFinished(qreal)), &particleFilter, SLOT(move(qreal)));
    QObject::connect(m_motionProxy, SIGNAL(turnFinished(qreal)), &particleFilter, SLOT(turn(qreal)));
    // TODO: Connect motionRequestFailed signal to motion planner
    QObject::connect(&particleFilter, SIGNAL(particlesMoved()), m_sensorManager, SLOT(requestNewData()));
    QObject::connect(&particleFilter, SIGNAL(particlesUpdated(QVector<Robot>)), &m_particleDisplay, SLOT(setParticles(QVector<Robot>)));

//    // TODO: MarkerProcessor is only used to print markers for debugging
//    QObject::connect(m_imageProcessor, SIGNAL(imageProcessed(QList<Marker>)), &markerProcessor, SLOT(processMarkers(QList<Marker>)));

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
    QObject::connect(m_sensorManager, SIGNAL(ready()), this, SLOT(checkIfReady()));
    QObject::connect(m_motionProxy, SIGNAL(ready()), this, SLOT(checkIfReady()));
    m_phoneUI.showExpanded();
#endif
    return true;
}

void Manager::mouseClicked()
{
    // TODO: Probably remove this
}

void Manager::checkIfReady()
{
    if (m_isReady)
        return;

    if (m_sensorManager->isReady() && m_motionProxy->isReady()) {
        m_isReady = true;
        emit ready();
        m_sensorManager->requestNewData();
    }
}
