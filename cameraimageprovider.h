#ifndef CAMERAIMAGEPROVIDER_H
#define CAMERAIMAGEPROVIDER_H

#include "imageprovider.h"

class CameraImageProvider : public ImageProvider
{
    Q_OBJECT
public:
    explicit CameraImageProvider(QObject *parent = 0);

    void init(QObject *camera);

    // slots
    void requestNextImage();
signals:

public slots:
    void processImageSaved(const QString& file);

private:
    QObject *m_camera;
};

#endif // CAMERAIMAGEPROVIDER_H
