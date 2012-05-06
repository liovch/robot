#ifndef CAMERAIMAGEPROVIDER_H
#define CAMERAIMAGEPROVIDER_H

#include <qcamera.h>
#include <qcameraimagecapture.h>
#include "imageprovider.h"

class CameraImageProvider : public ImageProvider
{
    Q_OBJECT
public:
    explicit CameraImageProvider(QObject *parent = 0);

    bool init();

    // slots
    void requestNextImage();

signals:

public slots:
    void processCapturedImage(int id, const QImage &preview);

private:
    QCamera *m_camera;
    QCameraImageCapture *m_imageCapture;
};

#endif // CAMERAIMAGEPROVIDER_H
