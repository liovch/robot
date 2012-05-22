#ifndef DECLARATIVECAMERAIMAGEPROVIDER_H
#define DECLARATIVECAMERAIMAGEPROVIDER_H

#include "imageprovider.h"

class DeclarativeCameraImageProvider : public ImageProvider
{
    Q_OBJECT
public:
    explicit DeclarativeCameraImageProvider(QObject *parent = 0);

    void init(QObject *camera);

    // slots
    void requestNextImage();
signals:

public slots:
    void processImageSaved(const QString& file);

private:
    QObject *m_camera;
};

#endif // DECLARATIVECAMERAIMAGEPROVIDER_H
