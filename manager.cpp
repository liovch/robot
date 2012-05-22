#include "manager.h"
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QDebug>
#include "settings.h"
#ifdef USE_AR_TOOLKIT
#include "artoolkitimageprocessor.h"
#else
#include "thresholdimageprocessor.h"
#endif
#ifndef MEEGO_EDITION_HARMATTAN
#include "particledisplay.h"
#include "folderimageprovider.h"
#else
#ifdef USE_DECLARATIVE_CAMERA
#include "declarativecameraimageprovider.h"
#else
#include "cameraimageprovider.h"
#endif // USE_DECLARATIVE_CAMERA
#endif // !MEEGO_EDITION_HARMATTAN

Manager::Manager(QObject *parent) :
    QObject(parent),
    m_imageProvider(0),
    m_imageProcessor(0),
    m_movementRequest(true)
{
}

bool Manager::init()
{
#ifndef MEEGO_EDITION_HARMATTAN
    imageViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    particleViewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    initMarkerParams();
    imageViewer.rootContext()->setContextProperty("markerParamsModel", QVariant::fromValue(gMarkerParams));
    imageViewer.setMainQmlFile(QLatin1String("qml/robot/imageviewer.qml"));

    qmlRegisterType<ParticleDisplay>("Robot", 1, 0, "ParticleDisplay");
    particleViewer.setMainQmlFile(QLatin1String("qml/robot/particleviewer.qml"));

    FolderImageProvider *imageProvider = new FolderImageProvider(this);
#ifdef USE_AR_TOOLKIT
    imageProvider->setDir("../../data/robot/artoolkit", "*.jpg");
#else
    imageProvider->setDir("../../data/robot/test", "*.jpg");
#endif

    QObject *imageViewerQML = imageViewer.rootObject()->findChild<QObject *>("image");
    Q_ASSERT(imageViewerQML);
    markerProcessor.setImageDisplay(imageViewerQML);
    markerProcessor.setOutputDirectory("../../data/robot/output");
#else
    m_phoneUI.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

#ifdef USE_DECLARATIVE_CAMERA
    m_phoneUI.setMainQmlFile("qml/robot/phone.qml");

    DeclarativeCameraImageProvider *imageProvider = new DeclarativeCameraImageProvider(this);
    QObject *camera = m_phoneUI.rootObject()->findChild<QObject *>(QString("camera"));
    Q_ASSERT(camera);
    imageProvider->init(camera);
#else
    m_phoneUI.setMainQmlFile("qml/robot/camera.qml");

    CameraImageProvider *imageProvider = new CameraImageProvider(this);
    if (!imageProvider->init()) {
        qDebug() << "Failed to initialize camera";
        return false;
    }
#endif // USE_DECLARATIVE_CAMERA
#endif // !MEEGO_EDITION_HARMATTAN

    m_imageProvider = imageProvider;

#ifdef USE_AR_TOOLKIT
    m_imageProcessor = new ARToolkitImageProcessor(this);
#else
    m_imageProcessor = new ThresholdImageProcessor(this);
#endif

    particleFilter.init(NUM_PARTICLES, MAX_POSITION);

    QObject::connect(m_imageProvider, SIGNAL(nextImage(QImage)), m_imageProcessor, SLOT(processImage(QImage)));
    QObject::connect(m_imageProcessor, SIGNAL(newMarkers(QList<Marker>)), &markerProcessor, SLOT(processMarkers(QList<Marker>)));
    QObject::connect(m_imageProcessor, SIGNAL(newMarkerMap(QVector<MarkerParams::MarkerId>,int,int)), &markerProcessor, SLOT(processMarkerMap(QVector<MarkerParams::MarkerId>,int,int)));
    QObject::connect(&movementProvider, SIGNAL(nextMovement(Movement)), &particleFilter, SLOT(move(Movement)));
    QObject::connect(m_imageProcessor, SIGNAL(newMarkers(QList<Marker>)), &particleFilter, SLOT(resample(QList<Marker>)));

#ifndef MEEGO_EDITION_HARMATTAN
    // connect QML components
    // connect mouse
    QObject::connect(imageViewerQML, SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));
    QObject::connect(particleViewer.rootObject(), SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));

#ifndef USE_AR_TOOLKIT
    // process image every time a parameter is changed
    foreach (QObject* obj, gMarkerParams) {
        MarkerParams *param = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(param);
        QObject::connect(param, SIGNAL(minRChanged()), m_imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(minGChanged()), m_imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(minBChanged()), m_imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxRChanged()), m_imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxGChanged()), m_imageProcessor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxBChanged()), m_imageProcessor, SLOT(processLastImage()));
    }
#endif
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
