#ifndef FOLDERIMAGEPROVIDER_H
#define FOLDERIMAGEPROVIDER_H

#include <QObject>
#include <QFileInfo>
#include "imageprovider.h"

class FolderImageProvider : public ImageProvider
{
    Q_OBJECT
public:
    explicit FolderImageProvider(QObject *parent = 0);

    void setDir(const QString& path, const QString& nameFilter);

public: // slots
    void requestNextImage();

private:
    QFileInfoList m_files; // TODO: For now just use predefined images from a folder
};

#endif // FOLDERIMAGEPROVIDER_H
