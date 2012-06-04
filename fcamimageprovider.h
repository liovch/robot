#ifndef FCAMIMAGEPROVIDER_H
#define FCAMIMAGEPROVIDER_H

#include <FCam/N9.h>
#include "imageprovider.h"

class FCamImageProvider : public ImageProvider
{
    Q_OBJECT
public:
    explicit FCamImageProvider(QObject *parent = 0);
    
    bool init();

    // slots
    void requestNextImage();

signals:
    
public slots:

private:
    void errorCheck();

    int m_index;
};

#endif // FCAMIMAGEPROVIDER_H
