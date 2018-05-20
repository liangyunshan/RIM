#include "panelbottomtoolbar.h"

#include <QHBoxLayout>
#include <QToolButton>

#include "head.h"
#include "constants.h"
#include "actionmanager/actionmanager.h"
#include "addfriend.h"
#include "maindialog.h"
#include "rsingleton.h"
#include "systemsettings.h"
#include "Network/netconnector.h"
#include "user/user.h"
#include "global.h"
#include "widget/rmessagebox.h"

#define PANEL_BOTTOM_TOOL_WIDTH 20
#define PANEL_BOTTOM_TOOL_HEIGHT 40

class PanelBottomToolBarPrivate : public GlobalData<PanelBottomToolBar>
{
    Q_DECLARE_PUBLIC(PanelBottomToolBar)

private:
    PanelBottomToolBarPrivate(PanelBottomToolBar * q):q_ptr(q)
    {
        addFriendInstance = NULL;
        settingsInstance = NULL;
        initWidget();
    }

    ~PanelBottomToolBarPrivate()
    {
        if(addFriendInstance)
        {
            addFriendInstance->close();
        }

        if(settingsInstance)
        {
            settingsInstance->close();
        }
    }

    void initWidget();

    PanelBottomToolBar * q_ptr;

    AddFriend * addFriendInstance;
    SystemSettings * settingsInstance;

    QWidget * mainWidget;
};

void PanelBottomToolBarPrivate::initWidget()
{
    mainWidget = new QWidget(q_ptr);
    mainWidget->setObjectName("Panel_Bottom_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(mainWidget);
    q_ptr->setLayout(mainLayout);

    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(6,1,1,1);
    contentLayout->setSpacing(2);

    RToolButton * toolButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_TOOL,q_ptr,SLOT(showSystemSetting()));
    toolButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    toolButton->setToolTip(QObject::tr("Main menu"));

    RToolButton * searchPerson = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_ADDPERSON,q_ptr,SLOT(showAddFriendPanel()));
    searchPerson->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    searchPerson->setToolTip(QObject::tr("Add Person"));

    RToolButton * notifyButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_NOTIFY,q_ptr,SLOT(showNotifyWindow()));
    notifyButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    notifyButton->setToolTip(QObject::tr("Notify windows"));

    RToolButton * fileServerButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_FILESERVER,q_ptr,SLOT(viewFileServerState()));
    fileServerButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    fileServerButton->setToolTip(QObject::tr("File server"));

    contentLayout->addWidget(toolButton);
    contentLayout->addWidget(searchPerson);
    contentLayout->addWidget(notifyButton);
    contentLayout->addWidget(fileServerButton);
    contentLayout->addStretch(1);

    mainWidget->setLayout(contentLayout);
}

PanelBottomToolBar::PanelBottomToolBar(QWidget *parent):
    d_ptr(new PanelBottomToolBarPrivate(this)),
    QWidget(parent)
{
    if(!d_ptr->settingsInstance)
    {
        d_ptr->settingsInstance = new SystemSettings();
        connect(d_ptr->settingsInstance,SIGNAL(destroyed(QObject*)),this,SLOT(updateSettingInstnce(QObject*)));
    }

    RSingleton<Subject>::instance()->attach(this);
}

PanelBottomToolBar::~PanelBottomToolBar()
{
    RSingleton<Subject>::instance()->detach(this);
    delete d_ptr;
}

void PanelBottomToolBar::onMessage(MessageType mtype)
{
    switch(mtype){
        case MESS_ADD_FRIEND_WINDOWS:
                showAddFriendPanel();
            break;
        case MESS_FILE_NET_ERROR:
            networkIsConnected(false);
            break;
        case MESS_FILE_NET_OK:
            networkIsConnected(true);
            break;
        default:
            break;
    }
}

void PanelBottomToolBar::showAddFriendPanel()
{
    MQ_D(PanelBottomToolBar);
    if(!d->addFriendInstance)
    {
        d->addFriendInstance = new AddFriend();
        connect(d->addFriendInstance,SIGNAL(destroyed(QObject*)),this,SLOT(updateFrinedInstance(QObject*)));
    }

    d->addFriendInstance->showNormal();
}

void PanelBottomToolBar::showSystemSetting()
{
    MQ_D(PanelBottomToolBar);
    if(!d->settingsInstance)
    {
        d->settingsInstance = new SystemSettings();
        connect(d->settingsInstance,SIGNAL(destroyed(QObject*)),this,SLOT(updateSettingInstnce(QObject*)));
    }

    d->settingsInstance->showNormal();
}

void PanelBottomToolBar::updateFrinedInstance(QObject *)
{
    MQ_D(PanelBottomToolBar);
    if(d->addFriendInstance)
    {
        d->addFriendInstance = NULL;
    }
}

void PanelBottomToolBar::updateSettingInstnce(QObject *)
{
    MQ_D(PanelBottomToolBar);
    if(d->settingsInstance)
    {
        d->settingsInstance = NULL;
    }
}

void PanelBottomToolBar::showNotifyWindow()
{
    RSingleton<Subject>::instance()->notify(MESS_NOTIFY_WINDOWS);
}

void PanelBottomToolBar::viewFileServerState()
{
    MQ_D(PanelBottomToolBar);
    if(!G_User->isFileOnLine()){
        if(FileNetConnector::instance())
            FileNetConnector::instance()->reconnect();
    }else{
        RMessageBox::information(nullptr,tr("information"),tr("File server connected!"),RMessageBox::Yes);
        //TODO 20180503 检测文件服务器连接状态
    }
}

void PanelBottomToolBar::networkIsConnected(bool connected)
{
    MQ_D(PanelBottomToolBar);
    RToolButton * butt = ActionManager::instance()->toolButton(Constant::TOOL_PANEL_FILESERVER);
    if(butt){
        if(connected){
            butt->setIcon(Constant::TOOL_PANEL_FILESERVER);
            butt->setToolTip(tr("File server connected"));
        }else{
            butt->setIcon(Constant::TOOL_PANEL_FILESERVER_ERROR);
            butt->setToolTip(tr("File server disconnected"));
        }
    }
}
