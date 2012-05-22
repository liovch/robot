#ifndef CAMERAIMAGEPROVIDER_H
#define CAMERAIMAGEPROVIDER_H

#include <qcamera.h>
#include <qcameraimagecapture.h>
#include <qmediarecorder.h>
#include "imageprovider.h"

class CameraImageProvider : public ImageProvider
{
    Q_OBJECT
public:
    explicit CameraImageProvider(QObject *parent = 0);
    ~CameraImageProvider();

    bool init();

    // slots
    void requestNextImage();

signals:

public slots:
    void processCapturedImage(int id, const QImage &preview);

private:
    QCamera *m_camera;
    QMediaRecorder *m_mediaRecorder;
    QCameraImageCapture *m_imageCapture;
};

#endif // CAMERAIMAGEPROVIDER_H
