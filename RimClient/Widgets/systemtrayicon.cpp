#include "systemtrayicon.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QDebug>

#include "head.h"
#include "constants.h"
#include "datastruct.h"

SystemTrayIcon * SystemTrayIcon::systemIcon = NULL;

class SystemTrayIconPrivate: public GlobalData<SystemTrayIcon>
{
    Q_DECLARE_PUBLIC(SystemTrayIcon)

public:
    SystemTrayIconPrivate();

    void setModel(SystemTrayIcon::SystemTrayModel mod){this->systemModel = mod;}
    SystemTrayIcon::SystemTrayModel model(){return this->systemModel;}

private:
    SystemTrayIcon::SystemTrayModel systemModel;
};

SystemTrayIconPrivate::SystemTrayIconPrivate()
{
    systemModel = SystemTrayIcon::System_Login;
}

SystemTrayIcon::SystemTrayIcon(QObject * parent):
    d_ptr(new SystemTrayIconPrivate()),
    QSystemTrayIcon(parent)
{
    systemIcon = this;
    initWidget();
}

SystemTrayIcon *SystemTrayIcon::instance()
{
    return systemIcon;
}

SystemTrayIcon::~SystemTrayIcon()
{
    if(systemIcon)
    {
        systemIcon = NULL;
    }
}

void SystemTrayIcon::setModel(SystemTrayIcon::SystemTrayModel model)
{
    MQ_D(SystemTrayIcon);
    d->setModel(model);

    if(model == System_Main)
    {
        if(!taryMainMenu)
        {
            taryMainMenu = new QMenu();

            lockAction = new QAction(tr("Lock"),taryMainMenu);
            lockAction->setIcon(QIcon(Constant::ICON_LOKC24));
            connect(lockAction,SIGNAL(triggered()),this,SIGNAL(lockPanel()));

            taryMainMenu->addSeparator();
            taryMainMenu->addAction(lockAction);
            taryMainMenu->addSeparator();
            taryMainMenu->addAction(mainPanelAction);
            taryMainMenu->addSeparator();
            taryMainMenu->addAction(exitAction);
        }
        connect(this,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(respIconActivated(QSystemTrayIcon::ActivationReason)));
        setContextMenu(taryMainMenu);
    }
    else if(model == System_Login)
    {
        setContextMenu(trayLoginMenu);
    }
}

SystemTrayIcon::SystemTrayModel SystemTrayIcon::model()
{
    MQ_D(SystemTrayIcon);
    return d->model();
}

void SystemTrayIcon::respIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    MQ_D(SystemTrayIcon);

    switch(reason)
    {
        case QSystemTrayIcon::DoubleClick:
                                          if(d->model() == System_Main)
                                          {
                                               emit showMainPanel();
                                          }
                                           break;
    }
}

void SystemTrayIcon::initWidget()
{
    taryMainMenu = NULL;

    trayLoginMenu = new QMenu();
    mainPanelAction = new QAction(tr("Open Panel"),trayLoginMenu);
    exitAction = new QAction(tr("Exit"),trayLoginMenu);
    exitAction->setIcon(QIcon(Constant::ICON_SIGN24));

    connect(mainPanelAction,SIGNAL(triggered()),this,SIGNAL(showMainPanel()));
    connect(exitAction,SIGNAL(triggered()),this,SIGNAL(quitApp()));

    trayLoginMenu->addSeparator();
    trayLoginMenu->addAction(mainPanelAction);
    trayLoginMenu->addSeparator();
    trayLoginMenu->addAction(exitAction);

    setContextMenu(trayLoginMenu);
    setToolTip(qApp->applicationName());
    setIcon(QIcon(Constant::ICON_SYSTEM24));
}
