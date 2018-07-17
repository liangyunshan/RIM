#include "splashlogindialog.h"

#ifdef __LOCAL_CONTACT__

#include <QTimer>
#include <QPixmap>
#include <QDir>
#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>

#include "global.h"
#include "widget/rmessagebox.h"
#include "file/xmlparse.h"
#include "util/rsingleton.h"
#include "constants.h"
#include "systemtrayicon.h"
#include "head.h"
#include "maindialog.h"
#include "Util/rutil.h"
#include "notifywindow.h"
#include "user/user.h"
#include "user/userclient.h"
#include "user/userfriendcontainer.h"
#include "systemnotifyview.h"
#include "chatpersonwidget.h"
#include "Network/netconnector.h"
#include "Widgets/actionmanager/actionmanager.h"
#include "thread/taskmanager.h"
#include "netsettings.h"
#include "messdiapatch.h"
#include "Util/rlog.h"
#include "Network/msgwrap/wrapfactory.h"
#include "../network/netglobal.h"
#include "others/serialno.h"

class SplashLoginDialogPrivate : public QObject,public GlobalData<SplashLoginDialog>
{
    Q_DECLARE_PUBLIC(SplashLoginDialog)
private:
    explicit SplashLoginDialogPrivate(SplashLoginDialog * q):q_ptr(q),
    trayIcon(nullptr),mainDialog(nullptr){
        initWidget();
    }

    void initWidget();

    SplashLoginDialog * q_ptr;

    QWidget * contentWidget;
    QWidget * iconWidget;
    ToolBar * toolBar;

    QPushButton *loginButt;

    QWidget * imageWidget;               /*!< logo */
    QLabel * nodeLabel;                  /*!< 节点号 */
    QLabel * tcpServerIp;                /*!< 服务器IP地址 */

    QLabel * tcpFlowServerIp1;           /*!< 串联服务器IP地址1 */

    SystemTrayIcon * trayIcon;
    MainDialog * mainDialog;
};

void SplashLoginDialogPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("AbstractWidget_ContentWidget");

    QVBoxLayout * contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    imageWidget = new QWidget(contentWidget);
    imageWidget->setObjectName("Logo_Widget");
    imageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    nodeLabel = new QLabel(imageWidget);
    nodeLabel->setObjectName("Node_Label");
    nodeLabel->setAlignment(Qt::AlignCenter);

    /***************信息设置区****************/
    QWidget * bottomWidget = new QWidget(contentWidget);

    QLabel * serverIp = new QLabel(bottomWidget);
    serverIp->setText(QObject::tr("Server Ip"));

    tcpServerIp = new QLabel(bottomWidget);

    QLabel * tandemtServerIp = new QLabel(bottomWidget);
    tandemtServerIp->setText(QObject::tr("Tandem Server IP"));

    tcpFlowServerIp1 = new QLabel(bottomWidget);

    loginButt = new QPushButton(bottomWidget);
    loginButt->setMinimumSize(QSize(0, 28));
    loginButt->setText(QObject::tr("Sin in"));
    loginButt->setObjectName("login");

    QGridLayout * bottomGridLayout = new QGridLayout;
    bottomGridLayout->setHorizontalSpacing(10);
    bottomGridLayout->setVerticalSpacing(10);
    bottomGridLayout->setContentsMargins(0,0,0,0);

    bottomGridLayout->setColumnStretch(0,3);
    bottomGridLayout->setColumnStretch(3,1);
    bottomGridLayout->setColumnStretch(6,3);

    bottomGridLayout->setRowStretch(0,1);
    bottomGridLayout->setRowStretch(5,1);

    bottomGridLayout->addWidget(serverIp,1,1,1,1);
    bottomGridLayout->addWidget(tcpServerIp,2,1,1,2);

    bottomGridLayout->addWidget(tandemtServerIp,1,4,1,1);
    bottomGridLayout->addWidget(tcpFlowServerIp1,2,4,1,2);

    bottomGridLayout->setRowStretch(3,1);
    bottomGridLayout->addWidget(loginButt,4,2,1,3);

    bottomWidget->setLayout(bottomGridLayout);

    contentLayout->addWidget(imageWidget);
    contentLayout->addWidget(bottomWidget);

    contentLayout->setStretchFactor(imageWidget,9);
    contentLayout->setStretchFactor(bottomWidget,7);

    contentWidget->setLayout(contentLayout);

    RToolButton * systemSetting = ActionManager::instance()->createToolButton(Id(Constant::TOOL_SETTING),q_ptr,SLOT(showNetSettings()));
    systemSetting->setToolTip(QObject::tr("System Setting"));

    RToolButton * minSize = ActionManager::instance()->createToolButton(Id(Constant::TOOL_MIN),q_ptr,SLOT(minsize()));
    minSize->setToolTip(QObject::tr("Min"));

    RToolButton * closeButt = ActionManager::instance()->createToolButton(Id(Constant::TOOL_CLOSE),q_ptr,SLOT(closeWindow()));
    closeButt->setToolTip(QObject::tr("Close"));

    toolBar = new ToolBar(contentWidget);
    toolBar->addStretch(1);
    toolBar->appendToolButton(systemSetting);
    toolBar->appendToolButton(minSize);
    toolBar->appendToolButton(closeButt);

    QObject::connect(loginButt,SIGNAL(pressed()),q_ptr,SLOT(prepareNetConnect()));

    q_ptr->setContentWidget(contentWidget);
    imageWidget->installEventFilter(q_ptr);
}

SplashLoginDialog::SplashLoginDialog(QWidget *parent):
    Widget(parent),d_ptr(new SplashLoginDialogPrivate(this))
{
    RSingleton<Subject>::instance()->attach(this);

    G_User = nullptr;

    QSize size = qApp->desktop()->screen()->size();
    setFixedSize(Constant::LOGIN_FIX_WIDTH,Constant::LOGIN_FIX_HEIGHT);
    setGeometry((size.width() - Constant::LOGIN_FIX_WIDTH)/2,(size.height() - Constant::LOGIN_FIX_HEIGHT)/2,Constant::LOGIN_FIX_WIDTH,Constant::LOGIN_FIX_HEIGHT);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    loadLocalSettings();

    connect(MessDiapatch::instance(),SIGNAL(textConnected(bool)),this,SLOT(respTextConnect(bool)));
    connect(MessDiapatch::instance(),SIGNAL(textSocketError()),this,SLOT(respTextSocketError()));
    connect(MessDiapatch::instance(),SIGNAL(transmitsInitialError(QString)),this,SLOT(respTransmitError(QString)));

    connect(MessDiapatch::instance(),SIGNAL(fileConnected(bool)),this,SLOT(respFileConnect(bool)));
    connect(MessDiapatch::instance(),SIGNAL(fileSocketError()),this,SLOT(respFileSocketError()));

    QTimer::singleShot(0,this,SLOT(initResource()));
}

SplashLoginDialog::~SplashLoginDialog()
{
    MQ_D(SplashLoginDialog);
    if(d->toolBar)
    {
        delete d->toolBar;
        d->toolBar = NULL;
    }
}

void SplashLoginDialog::onMessage(MessageType type)
{
    MQ_D(SplashLoginDialog);
    switch(type)
    {
        case MESS_SETTINGS:
            {
                d->trayIcon->setVisible(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,true).toBool());
                break;
            }
        case MESS_NOTIFY_WINDOWS:
            {
                RSingleton<NotifyWindow>::instance()->showMe();
                break;
            }
        default:
                break;
    }
}

void SplashLoginDialog::resizeEvent(QResizeEvent *)
{
    MQ_D(SplashLoginDialog);

    d->toolBar->setGeometry(WINDOW_MARGIN_SIZE,0,width() - WINDOW_MARGIN_SIZE * 3,d->toolBar->size().height());
}

void SplashLoginDialog::closeEvent(QCloseEvent *)
{
    qApp->exit();
}

bool SplashLoginDialog::eventFilter(QObject *watched, QEvent *event)
{
    MQ_D(SplashLoginDialog);
    if(watched == d->imageWidget){
        if(event->type() == QEvent::Resize){
            d->nodeLabel->setGeometry(d->imageWidget->width() - 70,d->imageWidget->height() - 40,70,40);
            return true;
        }
    }

    return Widget::eventFilter(watched,event);
}

/*!
 * @brief 解析本地配置信息
 * @note 配置信息来源于两部分: \n
 *       1.来源于716的配置文件，主要提供了节点号、通信列表等信息; \n
 *       2.自身配置文件，主要提供了服务器端ip地址、串联服务器ip地址等信息，可通过NetSettings类来保存、设置 \n
 */
void SplashLoginDialog::initResource()
{
    MQ_D(SplashLoginDialog);

    G_ParaSettings = new ParameterSettings::ParaSettings;
    QString localConfigName = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath) + QDir::separator() + QStringLiteral("参数配置.txt");
    if(!RSingleton<XMLParse>::instance()->parse(localConfigName,G_ParaSettings)){
        RMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Paramter settings read failed,system exit!"),
                             RMessageBox::Yes,RMessageBox::Yes);
        exit(-1);
    }

    //初始化传输链路!!!
    TextNetConnector::instance()->initialize();

    d->nodeLabel->setText(G_ParaSettings->baseInfo.nodeId);
}

/*!
 * @brief 查看系统通知消息
 * @param[in] info 通知消息内容
 * @return 无
 */
void SplashLoginDialog::viewSystemNotify(NotifyInfo info,int notifyCount)
{
    MQ_D(SplashLoginDialog);

    Q_UNUSED(notifyCount);
    if(info.type == NotifySystem)
    {
        ResponseFriendApply reqType = (ResponseFriendApply)info.ofriendResult;

        SystemNotifyView * view = new SystemNotifyView();
        connect(view,SIGNAL(chatWidth(QString)),this,SLOT(openChatDialog(QString)));

        if(info.stype == OperatePerson){
            //解决:AB同时向对方发送请求，A先接收B的请求，此时已是好友，但B后查看通知消息，造成状态不一致。
            if(reqType == FRIEND_REQUEST && RSingleton<UserFriendContainer>::instance()->containUser(info.accountId)){
                reqType = FRIEND_AGREE;
            }
        }else if(info.stype == OperateGroup){

        }

        view->setNotifyInfo(info);
        view->setNotifyType(reqType);
        view->show();
    }
    else if(info.type == NotifyUser)
    {
        UserClient * client = RSingleton<UserManager>::instance()->client(info.accountId);
        if(client)
        {
            if(client->chatPersonWidget == NULL)
            {
                client->chatPersonWidget = new ChatPersonWidget();
                client->chatPersonWidget->setUserInfo(client->simpleUserInfo);
                client->chatPersonWidget->setOuterNetConfig(client->netConfig);
                client->chatPersonWidget->initChatRecord();
            }
//            client->chatPersonWidget->showRecentlyChatMsg(notifyCount);
            client->chatPersonWidget->show();
        }
    }

    d->trayIcon->removeNotify(info.identityId);
}

void SplashLoginDialog::openChatDialog(QString accountId)
{
   UserClient * client = RSingleton<UserManager>::instance()->client(accountId);
   if(client->chatPersonWidget == NULL)
   {
       client->chatPersonWidget = new ChatPersonWidget();
       client->chatPersonWidget->setUserInfo(client->simpleUserInfo);
       client->chatPersonWidget->initChatRecord();
   }

   client->chatPersonWidget->show();
}

void SplashLoginDialog::showNetSettings()
{
    NetSettings * settings = new NetSettings(this);
    connect(settings,SIGNAL(ipInfoUpdated()),this,SLOT(loadLocalSettings()));
    settings->initLocalSettings();
    settings->show();
}

void SplashLoginDialog::minsize()
{
    MQ_D(SplashLoginDialog);
    if(d->trayIcon)
    {
        if(!d->trayIcon->isVisible())
        {
            d->trayIcon->setVisible(true);
        }
    }

    this->setVisible(false);
}

void SplashLoginDialog::closeWindow()
{
    RSingleton<TaskManager>::instance()->removeAll();
    this->close();
}

void SplashLoginDialog::prepareNetConnect()
{
    MQ_D(SplashLoginDialog);

    Q_UNUSED(d);
    G_NetSettings.connectedIpPort = G_NetSettings.textServer;
    TextNetConnector::instance()->connect();
    enableInput(false);
}

void SplashLoginDialog::respTextConnect(bool flag)
{
    MQ_D(SplashLoginDialog);

    if(!flag){
        RLOG_ERROR("Connect to server %s:%d error!",G_NetSettings.textServer.ip.toLocal8Bit().data(),G_NetSettings.textServer.port);
        RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Connect to text server error!"),RMessageBox::Yes);
        RSingleton<Subject>::instance()->notify(MESS_TEXT_NET_ERROR);
    }else{
        RSingleton<Subject>::instance()->notify(MESS_TEXT_NET_OK);
    }

    if(G_User == nullptr){
        UserBaseInfo baseInfo;
        baseInfo.accountId = G_ParaSettings->baseInfo.nodeId;
        baseInfo.nickName = G_ParaSettings->baseInfo.nodeId;

        G_User = new User(baseInfo);
    }

    if(G_User){
        G_User->setTextOnline(flag);
        G_User->setLogin(flag);

        if(!d->trayIcon)
        {
            d->trayIcon = new SystemTrayIcon();
            d->trayIcon->setModel(SystemTrayIcon::System_Login);
            d->trayIcon->setVisible(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,true).toBool());
            d->trayIcon->setModel(SystemTrayIcon::System_Main);
        }

        if(!d->trayIcon)
        {
            d->trayIcon = new SystemTrayIcon();
            d->trayIcon->setModel(SystemTrayIcon::System_Login);
            d->trayIcon->setVisible(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,true).toBool());
            d->trayIcon->setModel(SystemTrayIcon::System_Main);
        }

        if(!d->mainDialog){
            d->mainDialog = new MainDialog();
            RSingleton<NotifyWindow>::instance()->bindTrayIcon(d->trayIcon);
            connect(RSingleton<NotifyWindow>::instance(),SIGNAL(showSystemNotifyInfo(NotifyInfo,int)),this,SLOT(viewSystemNotify(NotifyInfo,int)));
            connect(RSingleton<NotifyWindow>::instance(),SIGNAL(ignoreAllNotifyInfo()),d->trayIcon,SLOT(removeAll()));
            connect(d->trayIcon,SIGNAL(showNotifyInfo(QString)),RSingleton<NotifyWindow>::instance(),SLOT(viewNotify(QString)));
        }

        if(flag)
        {
            d->mainDialog->setLogInState(STATUS_ONLINE);
        }
        else
        {
            d->mainDialog->setLogInState(STATUS_OFFLINE);
        }

        FileNetConnector::instance()->initialize();
        FileNetConnector::instance()->connect();

        unsigned short seriNo = SerialNo::instance()->getSerialNo();
        if(seriNo == 0)
        {
            SerialNo::instance()->updateSerialNo(1);
            SetSerialNo(1);
        }
        else
        {
            SetSerialNo(seriNo);
        }

        hide();
        d->mainDialog->show();

        if(flag){
            DataPackType request;
            request.msgType = MSG_CONTROL;
            request.msgCommand = MSG_TCP_TRANS;
            request.extendData.type495 = T_DATA_REG;
            request.extendData.usOrderNo = O_2051;
            request.extendData.usSerialNo = SERIALNO_FRASH;
            request.sourceId = G_User->BaseInfo().accountId;
            request.destId = request.sourceId;
            RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,C_TongKong,M_495);
        }
    }

    enableInput(true);
}

void SplashLoginDialog::respTextSocketError()
{
    if(G_User){
        G_User->setTextOnline(false);
    }
    G_NetSettings.connectedIpPort.setConnected(false);
    RSingleton<Subject>::instance()->notify(MESS_TEXT_NET_ERROR);
    RLOG_ERROR("Connect to server %s:%d error!",G_NetSettings.textServer.ip.toLocal8Bit().data(),G_NetSettings.textServer.port);
    RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Connect to text server error!"),RMessageBox::Yes);
}

void SplashLoginDialog::respTransmitError(QString errorMsg)
{
    RMessageBox::warning(this,QObject::tr("Warning"),errorMsg,RMessageBox::Yes);
}

void SplashLoginDialog::respFileConnect(bool flag)
{
    if(G_User){
        G_User->setFileOnline(flag);
    }

    if(flag)
    {
        RSingleton<Subject>::instance()->notify(MESS_FILE_NET_OK);
    }else{
        RLOG_ERROR("Connect to server %s:%d error!",G_NetSettings.textServer.ip.toLocal8Bit().data(),G_NetSettings.textServer.port);
        RSingleton<Subject>::instance()->notify(MESS_FILE_NET_ERROR);
        RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Connect to file server error!"),RMessageBox::Yes);
    }

    if(flag){
        DataPackType request;
        request.msgType = MSG_CONTROL;
        request.msgCommand = MSG_TCP_TRANS;
        request.extendData.type495 = T_DATA_REG;
        request.extendData.usOrderNo = O_2051;
        request.sourceId = G_User->BaseInfo().accountId;
        request.destId = request.sourceId;
        RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,C_TongKong,M_495,SERVER_FILE);
    }
}

void SplashLoginDialog::respFileSocketError()
{
    G_User->setFileOnline(false);
    RSingleton<Subject>::instance()->notify(MESS_FILE_NET_ERROR);
    RLOG_ERROR("Connect to server %s:%d error!",G_NetSettings.textServer.ip.toLocal8Bit().data(),G_NetSettings.textServer.port);
    RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Connect to file server error!"),RMessageBox::Yes);
}

void SplashLoginDialog::enableInput(bool flag)
{
    MQ_D(SplashLoginDialog);
    d->loginButt->setEnabled(flag);
    repolish(d->loginButt);
}

void SplashLoginDialog::loadLocalSettings()
{
    MQ_D(SplashLoginDialog);
    RUtil::globalSettings()->beginGroup(Constant::SYSTEM_NETWORK);
    G_NetSettings.textServer.ip = RUtil::globalSettings()->value(Constant::SYSTEM_NETWORK_TEXT_IP,Constant::DEFAULT_NETWORK_TEXT_IP).toString();
    G_NetSettings.textServer.port = RUtil::globalSettings()->value(Constant::SYSTEM_NETWORK_TEXT_PORT,Constant::DEFAULT_NETWORK_TEXT_PORT).toUInt();

    G_NetSettings.tandemServer.ip = RUtil::globalSettings()->value(Constant::SYSTEM_NETWORK_TANDEM_IP1,"").toString();
    G_NetSettings.tandemServer.port = RUtil::globalSettings()->value(Constant::SYSTEM_NETWORK_TANDEM_PORT1,"").toUInt();

    G_NetSettings.fileServer.ip = RUtil::globalSettings()->value(Constant::SYSTEM_NETWORK_FILE_IP,Constant::DEFAULT_NETWORK_FILE_IP).toString();
    G_NetSettings.fileServer.port = RUtil::globalSettings()->value(Constant::SYSTEM_NETWORK_FILE_PORT,Constant::DEFAULT_NETWORK_FILE_PORT).toUInt();

    RUtil::globalSettings()->endGroup();

    QString ipTemplate("%1:%2");
    d->tcpServerIp->setText(ipTemplate.arg(G_NetSettings.textServer.ip).arg(G_NetSettings.textServer.port));

    if(G_NetSettings.tandemServer.ip.size() > 0){
        d->tcpFlowServerIp1->setText(ipTemplate.arg(G_NetSettings.tandemServer.ip).arg(G_NetSettings.tandemServer.port));
    }else{
        d->tcpFlowServerIp1->clear();
    }
}

#endif
