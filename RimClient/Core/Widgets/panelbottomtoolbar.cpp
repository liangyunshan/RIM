#include "panelbottomtoolbar.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>

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
#include "netsettings.h"
#include "Network/msgwrap/wrapfactory.h"

#define PANEL_BOTTOM_TOOL_WIDTH 20
#define PANEL_BOTTOM_TOOL_HEIGHT 40

class PanelBottomToolBarPrivate : public GlobalData<PanelBottomToolBar>
{
    Q_DECLARE_PUBLIC(PanelBottomToolBar)

private:
    PanelBottomToolBarPrivate(PanelBottomToolBar * q):q_ptr(q),
        addFriendInstance(nullptr),settingsInstance(nullptr),
        netSettings(nullptr)
    {
        initWidget();
    }

    ~PanelBottomToolBarPrivate()
    {
        if(addFriendInstance)
            addFriendInstance->close();

        if(settingsInstance)
            settingsInstance->close();

        if(netSettings)
            netSettings->close();
    }

    void initWidget();

    PanelBottomToolBar * q_ptr;

    AddFriend * addFriendInstance;
    SystemSettings * settingsInstance;
    NetSettings * netSettings;

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

    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_BOTTOM_SETTING);

    QAction * generalAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_BOTTOM_GENERAL,q_ptr,SLOT(showSystemSetting()));
    generalAction->setText(QObject::tr("General setting"));

    QAction * netSettingAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_BOTTOM_NETSETTING,q_ptr,SLOT(showNetsettings()));
    netSettingAction->setText(QObject::tr("Network setting"));

    menu->addAction(generalAction);
    menu->addAction(netSettingAction);

    RToolButton * toolButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_TOOL,q_ptr,nullptr);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    toolButton->setMenu(menu);
    toolButton->setToolTip(QObject::tr("Main menu"));

#ifndef __LOCAL_CONTACT__
    RToolButton * searchPerson = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_ADDPERSON,q_ptr,SLOT(showAddFriendPanel()));
    searchPerson->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    searchPerson->setToolTip(QObject::tr("Add Person"));
#endif

    RToolButton * netConnectButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_NETWORK,q_ptr,nullptr);
    netConnectButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    netConnectButton->setToolTip(QObject::tr("Network connector"));

    QMenu * netConnectMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_BOTTOM_NETCONNECTOR);
    netConnectButton->setMenu(netConnectMenu);
    netConnectButton->setPopupMode(QToolButton::InstantPopup);

    RToolButton * fileServerButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_FILESERVER,q_ptr,SLOT(viewFileServerState()));
    fileServerButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    fileServerButton->setToolTip(QObject::tr("File server"));

    RToolButton * notifyButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_NOTIFY,q_ptr,SLOT(showNotifyWindow()));
    notifyButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    notifyButton->setToolTip(QObject::tr("Notify windows"));

    contentLayout->addWidget(toolButton);
#ifndef __LOCAL_CONTACT__
    contentLayout->addWidget(searchPerson);
#endif
    contentLayout->addWidget(notifyButton);

#ifdef __LOCAL_CONTACT__
    contentLayout->addWidget(netConnectButton);
#else
    contentLayout->addWidget(fileServerButton);
#endif
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
        connect(d_ptr->settingsInstance,SIGNAL(destroyed(QObject*)),this,SLOT(updateSettingInstance(QObject*)));
    }

    updateNetConnectorInfo();
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
        case MESS_TEXT_NET_OK:
            updateNetConnectorInfo();
            break;
        case MESS_TEXT_NET_ERROR:
            updateNetConnectorInfo();
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
        connect(d->settingsInstance,SIGNAL(destroyed(QObject*)),this,SLOT(updateSettingInstance(QObject*)));
    }

    d->settingsInstance->showNormal();
}

void PanelBottomToolBar::showNetsettings()
{
    MQ_D(PanelBottomToolBar);
    if(!d->netSettings){
        d->netSettings = new NetSettings();
        connect(d->netSettings,SIGNAL(destroyed(QObject*)),this,SLOT(updateNetSettingInstance(QObject*)));
        connect(d->netSettings,SIGNAL(ipInfoUpdated()),this,SLOT(updateNetConnectorInfo()));
        d->netSettings->enableMoveable(true);
    }
    d->netSettings->initLocalSettings();
    d->netSettings->showNormal();
}

void PanelBottomToolBar::updateNetConnectorInfo()
{
    QMenu * netConnectMenu = ActionManager::instance()->menu(Constant::MENU_PANEL_BOTTOM_NETCONNECTOR);
    if(netConnectMenu){
        netConnectMenu->clear();
        QVector<IpPort> ips = G_NetSettings.validIps();
        std::for_each(ips.begin(),ips.end(),[&](const IpPort & ip){
            QAction * action = new QAction();
            if(G_NetSettings.connectedIpPort == ip){
                if(G_NetSettings.connectedIpPort.isConnected())
                    action->setIcon(QPixmap(":/icon/resource/icon/Tool_Panel_Network_OK.png"));
                else
                    action->setIcon(QPixmap(":/icon/resource/icon/Tool_Panel_Network_Error.png"));
            }
            action->setText(QString("%1:%2").arg(ip.ip).arg(ip.port));
            connect(action,SIGNAL(triggered(bool)),this,SLOT(respChangeConnector(bool)));
            netConnectMenu->addAction(action);
        });
    }

    if(G_NetSettings.connectedIpPort.isConnected()){
        ActionManager::instance()->toolButton(Constant::TOOL_PANEL_NETWORK)->setToolTip(tr("Network connected"));
        ActionManager::instance()->action(Constant::ACTION_PANEL_BOTTOM_NETSETTING)->setIcon(QPixmap(":/icon/resource/icon/Tool_Panel_Network_OK.png"));
        ActionManager::instance()->toolButton(Constant::TOOL_PANEL_NETWORK)->QToolButton::setIcon(QPixmap(":/icon/resource/icon/Tool_Panel_Network_OK.png"));
    }else{
        ActionManager::instance()->toolButton(Constant::TOOL_PANEL_NETWORK)->setToolTip(tr("Network disconnected"));
        ActionManager::instance()->action(Constant::ACTION_PANEL_BOTTOM_NETSETTING)->setIcon(QPixmap(":/icon/resource/icon/Tool_Panel_Network_Error.png"));
        ActionManager::instance()->toolButton(Constant::TOOL_PANEL_NETWORK)->QToolButton::setIcon(QPixmap(":/icon/resource/icon/Tool_Panel_Network_Error.png"));
    }
}

/*!
 * @brief 切换网络连接属性
 * @note 网络已经连接之后，
 * @param[in] bool
 */
void PanelBottomToolBar::respChangeConnector(bool)
{
    QAction * toolButt = dynamic_cast<QAction *>(QObject::sender());
    if(toolButt){
       QStringList connectorInfo = toolButt->text().split(":");
       if(connectorInfo.size() == 2){
            if(G_NetSettings.connectedIpPort.isConnected()){
                RMessageBox::warning(this,QObject::tr("warning"),tr("Network established!"),RMessageBox::Yes);
            }else{
                RMessageBox::information(this,QObject::tr("information"),tr("Attempt to connect %1:%2").arg(connectorInfo.at(0))
                                         .arg(connectorInfo.at(1)),RMessageBox::Yes);

                G_NetSettings.connectedIpPort = IpPort(connectorInfo.at(0),connectorInfo.at(1).toUShort());
                TextNetConnector::instance()->connect();

                //重新注册
                DataPackType request;
                request.msgType = MSG_CONTROL;
                request.msgCommand = MSG_TCP_TRANS;
                request.extendData.type495 = T_DATA_REG;
                request.extendData.usOrderNo = O_2051;
                request.sourceId = G_User->BaseInfo().accountId;
                request.destId = request.sourceId;
                RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,C_TongKong,M_495);
            }
       }
    }
}

void PanelBottomToolBar::updateFrinedInstance(QObject *)
{
    MQ_D(PanelBottomToolBar);
    if(d->addFriendInstance)
        d->addFriendInstance = nullptr;
}

void PanelBottomToolBar::updateSettingInstance(QObject *)
{
    MQ_D(PanelBottomToolBar);
    if(d->settingsInstance)
        d->settingsInstance = nullptr;
}

void PanelBottomToolBar::updateNetSettingInstance(QObject *)
{
    MQ_D(PanelBottomToolBar);
    if(d->netSettings)
        d->netSettings = nullptr;
}

void PanelBottomToolBar::showNotifyWindow()
{
    RSingleton<Subject>::instance()->notify(MESS_NOTIFY_WINDOWS);
}

void PanelBottomToolBar::updateNetConnector()
{

}

void PanelBottomToolBar::viewFileServerState()
{
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
