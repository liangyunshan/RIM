#include "application.h"

#include <QDebug>

#include "messdiapatch.h"

Application::Application(int &argc, char **argv):
    QApplication(argc,argv)
{

}

bool Application::notify(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::ScreenChangeInternal || event->type() == QEvent::OrientationChange)
    {
        MessDiapatch::instance()->onScreenChanged();
    }

    return QApplication::notify(obj,event);
}
