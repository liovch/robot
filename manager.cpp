#include "manager.h"
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include <QDebug>
#include "settings.h"
#include "artoolkitimageprocessor.h"
#ifndef MEEGO_EDITION_HARMATTAN
    #include "particledisplay.h"
    #include "folderimageprovider.h"
#else
    #include "fcamimageprovider.h"
#endif

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
    // TODO: Need new image viewer?
//    imageViewer.rootContext()->setContextProperty("markerParamsModel", QVariant::fromValue(gMarkerParams));
    imageViewer.setMainQmlFile(QLatin1String("qml/robot/imageviewer.qml"));

    qmlRegisterType<ParticleDisplay>("Robot", 1, 0, "ParticleDisplay");
    particleViewer.setMainQmlFile(QLatin1String("qml/robot/particleviewer.qml"));

    FolderImageProvider *imageProvider = new FolderImageProvider(this);
    imageProvider->setDir("../../data/robot/artoolkit", "*.jpg");

    QObject *imageViewerQML = imageViewer.rootObject()->findChild<QObject *>("image");
    Q_ASSERT(imageViewerQML);
#else
    m_phoneUI.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    m_phoneUI.setMainQmlFile("qml/robot/camera.qml");

    FCamImageProvider *imageProvider = new FCamImageProvider(this);
    if (!imageProvider->init()) {
        qDebug() << "Failed to initialize camera";
        return false;
    }
#endif // !MEEGO_EDITION_HARMATTAN

    m_imageProvider = imageProvider;
    m_imageProcessor = new ARToolkitImageProcessor(this);

    particleFilter.init(NUM_PARTICLES, MAX_POSITION);

    QObject::connect(m_imageProvider, SIGNAL(nextImage(QImage)), m_imageProcessor, SLOT(processImage(QImage)));
    QObject::connect(m_imageProcessor, SIGNAL(imageProcessed(QList<Marker>)), &markerProcessor, SLOT(processMarkers(QList<Marker>)));
    QObject::connect(m_imageProcessor, SIGNAL(newMarkerMap(QVector<MarkerParams::MarkerId>,int,int)), &markerProcessor, SLOT(processMarkerMap(QVector<MarkerParams::MarkerId>,int,int)));
    QObject::connect(&movementProvider, SIGNAL(nextMovement(Movement)), &particleFilter, SLOT(move(Movement)));
    QObject::connect(m_imageProcessor, SIGNAL(imageProcessed(QList<Marker>)), &particleFilter, SLOT(resample(QList<Marker>)));

#ifndef MEEGO_EDITION_HARMATTAN
    // connect QML components
    // connect mouse
    QObject::connect(imageViewerQML, SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));
    QObject::connect(particleViewer.rootObject(), SIGNAL(qmlClicked()), this, SLOT(mouseClicked()));

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
