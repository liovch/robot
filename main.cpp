#include <QtGui/QApplication>
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
    viewer.setMainQmlFile(QLatin1String("qml/robot/main.qml"));

    QDeclarativeItem* rootObject = qobject_cast<QDeclarativeItem*>(viewer.rootObject());
    QObject *qmlImage = rootObject->findChild<QObject *>(QString("image"));
    if (!qmlImage) {
        qDebug() << "QML image not found";
        return -1;
    }

    markerProcessor.setImageDisplay(qmlImage);
    markerProcessor.setOutputDirectory("/Volumes/Working/liovch/data/robot/output");

    QObject::connect(rootObject, SIGNAL(qmlClicked()), &provider, SLOT(requestNextImage()));
    provider.requestNextImage(); // request first image

    viewer.showExpanded();
    return app->exec();
}
