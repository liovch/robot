#include <QtGui/QApplication>
#include <QDebug>
#include "manager.h"
#include "qmlapplicationviewer.h"
#include <QtMobility/qmobilityglobal.h>
#include <QtSystemInfo/QSystemScreenSaver>

QTM_BEGIN_NAMESPACE
class QSystemScreenSaver;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication* app = createApplication(argc, argv);

    QSystemScreenSaver screenSaver;
    screenSaver.setScreenSaverInhibit();

    Manager manager;
    if (!manager.init())
        return -1;

    return app->exec();
}
