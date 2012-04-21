#include <QtGui/QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeItem>
#include "qmlapplicationviewer.h"
#include "imageprocessor.h"
#include "imageprovider.h"
#include "markerprocessor.h"
#include <QDebug>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    ImageProvider provider;
    provider.setDir("/Volumes/Working/liovch/data/robot/test_N9", "*.jpg");

    ImageProcessor processor;
    QObject::connect(&provider, SIGNAL(nextImage(QImage)), &processor, SLOT(processImage(QImage)));

    MarkerProcessor markerProcessor;
    QObject::connect(&processor, SIGNAL(newMarkers(QList<Marker>)), &markerProcessor, SLOT(processMarkers(QList<Marker>)));
    QObject::connect(&processor, SIGNAL(newMarkerMap(QVector<MarkerParams::MarkerId>,int,int)), &markerProcessor, SLOT(processMarkerMap(QVector<MarkerParams::MarkerId>,int,int)));

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    //qmlRegisterType<MarkerParams>("Robot", 1, 0, "MarkerParams");

    // setup the model
    viewer.rootContext()->setContextProperty("markerParamsModel", QVariant::fromValue(gMarkerParams));

    viewer.setMainQmlFile(QLatin1String("qml/robot/main.qml"));

    QDeclarativeItem* object = qobject_cast<QDeclarativeItem*>(viewer.rootObject());
    object = object->findChild<QDeclarativeItem *>(QString("imageRect"));
    QObject *qmlImage = object->findChild<QObject *>(QString("image"));
    if (!qmlImage) {
        qDebug() << "QML image not found";
        return -1;
    }

    markerProcessor.setImageDisplay(qmlImage);
    markerProcessor.setOutputDirectory("/Volumes/Working/liovch/data/robot/output");

    // connect signals
    // switch to next image when image control is clicked
    QObject::connect(object, SIGNAL(qmlClicked()), &provider, SLOT(requestNextImage()));
    // process image every time a parameter is changed
    foreach (QObject* obj, gMarkerParams) {
        MarkerParams *param = qobject_cast<MarkerParams*>(obj);
        Q_ASSERT(param);
        QObject::connect(param, SIGNAL(minRChanged()), &processor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(minGChanged()), &processor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(minBChanged()), &processor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxRChanged()), &processor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxGChanged()), &processor, SLOT(processLastImage()));
        QObject::connect(param, SIGNAL(maxBChanged()), &processor, SLOT(processLastImage()));    }

    provider.requestNextImage(); // request first image

    viewer.showExpanded();
    return app->exec();
}
