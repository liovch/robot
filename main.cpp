#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include "imageprocessor.h"
#include <QDebug>

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    initMarkerParams();
    QImage image("/Volumes/Working/liovch/data/robot/test_N9/00_0mm_.jpg");
    if (!image.isNull()) {
        ImageProcessor imageProcessor;
        QList<Marker> markers = imageProcessor.processImage(image);
        foreach (Marker marker, markers) {
            qDebug() << marker.id() << marker.distance();
        }
    }

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/robot/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
