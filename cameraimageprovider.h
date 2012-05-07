#ifndef CAMERAIMAGEPROVIDER_H
#define CAMERAIMAGEPROVIDER_H

#include "imageprovider.h"
#include <QDeclarativeEngine>

class CameraImageProvider : public ImageProvider
{
    Q_OBJECT
public:
    explicit CameraImageProvider(QObject *parent = 0);

    void init(QObject *camera, QDeclarativeEngine *engine);

    // slots
    void requestNextImage();
signals:

public slots:
    void processCapturedImage(const QString& preview);

private:
    QObject *m_camera;
    QDeclarativeEngine *m_engine;
};

#endif // CAMERAIMAGEPROVIDER_H
