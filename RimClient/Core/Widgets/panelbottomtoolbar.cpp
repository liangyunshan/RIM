#include "panelbottomtoolbar.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>

#include "head.h"
#include "constants.h"
#include "actionmanager/actionmanager.h"
#include "addfriend.h"
#include "maindialog.h"
#include "util/rsingleton.h"
#include "systemsettings.h"
#include "Network/netconnector.h"
#include "user/user.h"
#include "global.h"
#include "widget/rmessagebox.h"
#include "netsettings.h"
#include "Network/msgwrap/wrapfactory.h"
#include "../file/globalconfigfile.h"

#define PANEL_BOTTOM_TOOL_WIDTH 20
#define PANEL_BOTTOM_TOOL_HEIGHT 40

const char IP_TYPE[]  = "TP_TYPE";
const char IP_DATA[]  = "TP_DATA";

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
#ifndef __LOCAL_CONTACT__
    RToolButton * fileServerButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_FILESERVER,q_ptr,SLOT(viewFileServerState()));
    fileServerButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    fileServerButton->setToolTip(QObject::tr("File server"));
#endif
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
        case MESS_TEXT_NET_ERROR:
            updateNetConnectorInfo();
            break;

        case MESS_FILE_NET_ERROR:
#ifdef __LOCAL_CONTACT__
            updateNetConnectorInfo();
#else
            networkIsConnected(false);
#endif
            break;
        case MESS_FILE_NET_OK:
#ifdef __LOCAL_CONTACT__
            updateNetConnectorInfo();
#else
            networkIsConnected(true);
#endif
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

        std::function<void(const IpPort &,ServerType,IpPort connectedIpPort)> func = [&](const IpPort & ip,ServerType ipType,IpPort connectedIpPort){
            QAction * action = new QAction();
            if(connectedIpPort == ip){
                if(connectedIpPort.isConnected())
                    action->setIcon(QPixmap(":/icon/resource/icon/Tool_Panel_Network_OK.png"));
                else
                    action->setIcon(QPixmap(":/icon/resource/icon/Tool_Panel_Network_Error.png"));
            }
            action->setProperty(IP_TYPE,ipType);
            action->setProperty(IP_DATA,QString("%1:%2").arg(ip.ip).arg(ip.port));

            if(ipType == SERVER_TEXT)
                action->setText(QString("%1:%2 | %3").arg(ip.ip).arg(ip.port).arg(tr("Text Server")));
            else if(ipType == SERVER_FILE)
                action->setText(QString("%1:%2 | %3").arg(ip.ip).arg(ip.port).arg(tr("File Server")));

            connect(action,SIGNAL(triggered(bool)),this,SLOT(respChangeConnector(bool)));
            netConnectMenu->addAction(action);
        };

        //文本服务器
        QVector<IpPort> ips = Global::G_GlobalConfigFile->netSettings.validTextIps();
        std::for_each(ips.begin(),ips.end(),std::bind(func,std::placeholders::_1,SERVER_TEXT,Global::G_GlobalConfigFile->netSettings.connectedTextIpPort));

        netConnectMenu->addSeparator();

        //文件服务器
#ifndef __LOCAL_CONTACT__
        ips = Global::G_GlobalConfigFile->netSettings.validFileIps();
        std::for_each(ips.begin(),ips.end(),std::bind(func,std::placeholders::_1,SERVER_FILE,Global::G_GlobalConfigFile->netSettings.connectedFileIpPort));
#endif
    }

    if(Global::G_GlobalConfigFile->netSettings.connectedTextIpPort.isConnected()
#ifndef __LOCAL_CONTACT__
     && Global::G_GlobalConfigFile->netSettings.connectedFileIpPort.isConnected()
#endif
            ){
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
       ServerType ipType = static_cast<ServerType>(toolButt->property(IP_TYPE).toInt());

       QStringList connectorInfo = toolButt->property(IP_DATA).toString().split(":");
       if(connectorInfo.size() == 2){
            if(ipType == SERVER_TEXT){
                if(Global::G_GlobalConfigFile->netSettings.connectedTextIpPort.isConnected()){
                    RMessageBox::warning(this,QObject::tr("warning"),tr("Text Network established!"),RMessageBox::Yes);
                }else{
                    RMessageBox::information(this,QObject::tr("information"),tr("Attempt to connect %1:%2").arg(connectorInfo.at(0))
                                             .arg(connectorInfo.at(1)),RMessageBox::Yes);

                    Global::G_GlobalConfigFile->netSettings.connectedTextIpPort = IpPort(connectorInfo.at(0),connectorInfo.at(1).toUShort());
                    TextNetConnector::instance()->connect();
                }
            }else if(ipType == SERVER_FILE){
#ifndef __LOCAL_CONTACT__
                if(Global::G_GlobalConfigFile->netSettings.connectedFileIpPort.isConnected()){
                    RMessageBox::warning(this,QObject::tr("warning"),tr("File Network established!"),RMessageBox::Yes);
                }else{
                    RMessageBox::information(this,QObject::tr("information"),tr("Attempt to connect %1:%2").arg(connectorInfo.at(0))
                                             .arg(connectorInfo.at(1)),RMessageBox::Yes);

                    Global::G_GlobalConfigFile->netSettings.connectedFileIpPort = IpPort(connectorInfo.at(0),connectorInfo.at(1).toUShort());
                    FileNetConnector::instance()->connect();
                }
#endif
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

#ifndef __LOCAL_CONTACT__
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
#endif

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
