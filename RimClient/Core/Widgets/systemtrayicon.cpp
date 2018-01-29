#include "systemtrayicon.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QTimer>

#include "head.h"
#include "constants.h"
#include "datastruct.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"

SystemTrayIcon * SystemTrayIcon::systemIcon = NULL;

class SystemTrayIconPrivate: public GlobalData<SystemTrayIcon>
{
    Q_DECLARE_PUBLIC(SystemTrayIcon)

public:
    SystemTrayIconPrivate(SystemTrayIcon * q);

    void setModel(SystemTrayIcon::SystemTrayModel mod){this->systemModel = mod;}
    SystemTrayIcon::SystemTrayModel model(){return this->systemModel;}

private:
    void initWidget();

private:
    SystemTrayIcon * q_ptr;
    SystemTrayIcon::SystemTrayModel systemModel;

    QMenu * trayLoginMenu;
    QMenu * taryMainMenu;
    QAction * exitAction;
    QAction * mainPanelAction;

    QAction * lockAction;
    QTimer * blinkingTimer;

    QIcon blinkingIcon;
    int notifyCount;

    SystemTrayIcon::NotifyModel notifyModel;
};

SystemTrayIconPrivate::SystemTrayIconPrivate(SystemTrayIcon *q):
    q_ptr(q)
{
    systemModel = SystemTrayIcon::System_Login;
    blinkingTimer = NULL;
    notifyCount = 0;
    initWidget();
}

void SystemTrayIconPrivate::initWidget()
{
    taryMainMenu = NULL;

    trayLoginMenu = new QMenu();
    mainPanelAction = new QAction(QObject::tr("Open Panel"),trayLoginMenu);
    exitAction = new QAction(QObject::tr("Exit"),trayLoginMenu);
    exitAction->setIcon(QIcon(Constant::ICON_SIGN24));

    QObject::connect(mainPanelAction,SIGNAL(triggered()),q_ptr,SIGNAL(showMainPanel()));
    QObject::connect(exitAction,SIGNAL(triggered()),q_ptr,SIGNAL(quitApp()));

    trayLoginMenu->addSeparator();
    trayLoginMenu->addAction(mainPanelAction);
    trayLoginMenu->addSeparator();
    trayLoginMenu->addAction(exitAction);

    q_ptr->setContextMenu(trayLoginMenu);
}

SystemTrayIcon::SystemTrayIcon(QObject * parent):
    d_ptr(new SystemTrayIconPrivate(this)),
    QSystemTrayIcon(parent)
{
    systemIcon = this;

    setToolTip(qApp->applicationName());

    notify(NoneNotify);
}

SystemTrayIcon *SystemTrayIcon::instance()
{
    return systemIcon;
}

SystemTrayIcon::~SystemTrayIcon()
{
    MQ_D(SystemTrayIcon);
    if(systemIcon)
    {
        systemIcon = NULL;
    }

    if(d_ptr)
    {
        if(d->blinkingTimer)
        {
            if(d->blinkingTimer->isActive())
            {
                d->blinkingTimer->stop();
            }

            delete d->blinkingTimer;
        }
    }
}

void SystemTrayIcon::setModel(SystemTrayIcon::SystemTrayModel model)
{
    MQ_D(SystemTrayIcon);
    d->setModel(model);

    if(model == System_Main)
    {
        if(!d->taryMainMenu)
        {
            d->taryMainMenu = new QMenu();

            d->lockAction = new QAction(tr("Lock"),d->taryMainMenu);
            d->lockAction->setIcon(QIcon(Constant::ICON_LOKC24));
            connect(d->lockAction,SIGNAL(triggered()),this,SIGNAL(lockPanel()));

            d->taryMainMenu->addSeparator();
            d->taryMainMenu->addAction(d->lockAction);
            d->taryMainMenu->addSeparator();
            d->taryMainMenu->addAction(d->mainPanelAction);
            d->taryMainMenu->addSeparator();
            d->taryMainMenu->addAction(d->exitAction);
        }
        connect(this,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(respIconActivated(QSystemTrayIcon::ActivationReason)));
        setContextMenu(d->taryMainMenu);
    }
    else if(model == System_Login)
    {
        setContextMenu(d->trayLoginMenu);
    }
}

void SystemTrayIcon::notify(SystemTrayIcon::NotifyModel model, QString imagePath)
{
    MQ_D(SystemTrayIcon);
    d->notifyModel = model;
    d->notifyCount = 0;

    switch(model)
    {
        case NoneNotify:
                        {
                            if(d->blinkingTimer && d->blinkingTimer->isActive())
                            {
                                d->blinkingTimer->stop();
                            }
                            d->blinkingIcon = QIcon(RSingleton<ImageManager>::instance()->getWindowIcon());
                            setIcon(d->blinkingIcon);
                            break;
                        }
        case SystemNotify:
                        {
                            d->blinkingIcon = QIcon(RSingleton<ImageManager>::instance()->getIcon(ImageManager::ICON_SYSTEMNOTIFY,ImageManager::ICON_24));
                            startBliking();
                            break;
                        }
        case UserNotify:
                        {
                            d->blinkingIcon = QIcon(imagePath);
                            startBliking();
                            break;
                        }
        default:
            break;
    }
}

void SystemTrayIcon::startBliking(int interval)
{
    MQ_D(SystemTrayIcon);

    if(d->blinkingTimer == NULL)
    {
        d->blinkingTimer = new QTimer();
        connect(d->blinkingTimer,SIGNAL(timeout()),this,SLOT(switchNotifyImage()));
    }

    if(!d->blinkingTimer->isActive())
    {
        d->blinkingTimer->start(interval);
    }
}

void SystemTrayIcon::switchNotifyImage()
{
    MQ_D(SystemTrayIcon);
    d->notifyCount++ %2 == 0?setIcon(d->blinkingIcon):setIcon(QIcon());
}

SystemTrayIcon::SystemTrayModel SystemTrayIcon::model()
{
    MQ_D(SystemTrayIcon);
    return d->model();
}

void SystemTrayIcon::respIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    MQ_D(SystemTrayIcon);

    qDebug()<<"+++++:"<<(int)reason;

    switch(reason)
    {
        case QSystemTrayIcon::DoubleClick:
                                          if(d->model() == System_Main)
                                          {
                                               emit showMainPanel();
                                          }
                                           break;
    case QSystemTrayIcon::Trigger:
                                            {
                                            }
    break;
        default:
            break;
    }
}
