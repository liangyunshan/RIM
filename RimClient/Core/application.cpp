#include "application.h"

#include <QDebug>

#include "Widgets/systemtrayicon.h"

Application::Application(int &argc, char **argv):
    QApplication(argc,argv)
{

}

bool Application::notify(QObject *obj, QEvent *event)
{
    if(SystemTrayIcon::instance() == obj)
    {
    }

    return QApplication::notify(obj,event);
}
