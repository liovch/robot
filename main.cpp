#include <QtGui/QApplication>
#include <QDebug>
#include "manager.h"
#include "qmlapplicationviewer.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication* app = createApplication(argc, argv);

    Manager manager;
    if (!manager.init())
        return -1;

    return app->exec();
}
