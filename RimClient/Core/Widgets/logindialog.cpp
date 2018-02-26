#include "logindialog.h"

#include <QList>
#include <QToolButton>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>
#include <QListView>
#include <QMenu>
#include <QAction>
#include <QDesktopWidget>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QApplication>
#include <QDir>
#include <QListWidget>

#include "head.h"
#include "datastruct.h"
#include "toolbar.h"
#include "constants.h"
#include "onlinestate.h"
#include "rsingleton.h"
#include "user/userinfofile.h"
#include "Util/rutil.h"
#include "Util/rlog.h"
#include "Util/imagemanager.h"
#include "maindialog.h"
#include "systemtrayicon.h"
#include "Widgets/actionmanager/actionmanager.h"
#include "thread/taskmanager.h"
#include "Network/msgwrap.h"
#include "Network/netconnector.h"
#include "widget/rmessagebox.h"
#include "registdialog.h"
#include "netsettings.h"
#include "global.h"
#include "widget/rlabel.h"
#include "messdiapatch.h"
#include "media/mediaplayer.h"
#include "notifywindow.h"
#include "systemnotifyview.h"
#include "user/userclient.h"
#include "abstractchatwidget.h"
#include "sql/sqlprocess.h"
#include "sql/databasemanager.h"
#include "widget/rcombobox.h"
#include "json/jsonresolver.h"
using namespace TextUnit ;

class LoginDialogPrivate : public QObject,public GlobalData<LoginDialog>
{
    Q_DECLARE_PUBLIC(LoginDialog)
public:
    LoginDialogPrivate(LoginDialog * q):q_ptr(q)
    {
        initWidget();

        numberExp = QRegExp(Constant::AccountId_Reg);
        passExp = QRegExp(Constant::AccountPassword_Reg);

        notifyWindow = NULL;

        isSystemUserIcon = true;
        isNewUser = true;
    }
    ~LoginDialogPrivate(){}

    void initWidget();

private:
    LoginDialog * q_ptr;

    int userIndex(QString text);

    QList<UserInfoDesc *> localUserInfo;

    QPoint mousePreseePoint;

    bool isSystemUserIcon;
    QString defaultUserIconPath;

    bool isNewUser;

    QWidget * contentWidget;
    QWidget * iconWidget;

    RIconLabel *userIcon;
    RTextLabel *forgetPassword;
    QPushButton *login;
    RTextLabel *regist;
    QLineEdit *password;
    QCheckBox *rememberPassord;
    QCheckBox *autoLogin;
    RComboBox *userList;
    QListWidget * userListWidget;

    QRegExp numberExp;
    QRegExp passExp;

    ToolBar * toolBar;
    OnLineState * onlineState;
    QToolButton * systemSetting;
    SystemTrayIcon * trayIcon;
    MainDialog * mainDialog;

    NotifyWindow * notifyWindow;
};

void LoginDialogPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("AbstractWidget_ContentWidget");

    QVBoxLayout * contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    QWidget * imageWidget = new QWidget(contentWidget);
    imageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageWidget->setStyleSheet("border-image:url(':/background/resource/background/login.png')");

    /***************头像显示区****************/
    QWidget * bottomWidget = new QWidget(contentWidget);

    iconWidget = new QWidget(bottomWidget);

    QVBoxLayout * iconLayout = new QVBoxLayout(iconWidget);
    iconLayout->setContentsMargins(5,0,5,0);

    userIcon = new RIconLabel(iconWidget);
    userIcon->setTransparency(true);
    userIcon->setMinimumSize(QSize(80, 80));
    userIcon->setMaximumSize(QSize(80, 80));
    iconLayout->addWidget(userIcon);

    QSpacerItem * iconSpacer = new QSpacerItem(50, 20, QSizePolicy::Fixed, QSizePolicy::Expanding);

    iconLayout->addItem(iconSpacer);

    onlineState = new OnLineState(iconWidget);
    onlineState->setStyleSheet("background-color:rgba(0,0,0,0)");

    iconWidget->setLayout(iconLayout);

    /*****************登陆输入区******************/
    QWidget * inputWidget = new QWidget();
    QGridLayout * inputGridLayout = new QGridLayout;

    inputGridLayout->setHorizontalSpacing(10);
    inputGridLayout->setVerticalSpacing(2);
    inputGridLayout->setContentsMargins(0, 0, 0, 0);

    QRegExpValidator * numberValidator = new QRegExpValidator(QRegExp(Constant::AccountId_Reg));

    userListWidget = new QListWidget();

    userList = new RComboBox();
    userList->setObjectName("LoginComboBox");
    userList->setFixedSize(QSize(193, 28));
    userList->setModel(userListWidget->model());
    userList->setView(userListWidget);
    userList->setEditable(true);
    userList->lineEdit()->setValidator(numberValidator);
    userList->lineEdit()->setPlaceholderText(QObject::tr("Input number"));

    QRegExpValidator * passValidator = new QRegExpValidator(QRegExp(Constant::AccountPassword_Reg));
    password = new QLineEdit();
    password->setFixedSize(QSize(193, 28));
    password->setValidator(passValidator);
    password->setPlaceholderText(QObject::tr("Input password"));
    password->setEchoMode(QLineEdit::Password);

    login = new QPushButton();
    login->setMinimumSize(QSize(0, 28));
    login->setText(QObject::tr("Sin in"));

    forgetPassword = new RTextLabel();
    forgetPassword->setText(QObject::tr("Forget Password"));

    regist = new RTextLabel();
    regist->setText(QObject::tr("Sin up"));
    QObject::connect(regist,SIGNAL(mousePress()),q_ptr,SLOT(showRegistDialog()));

    QWidget * widget_4 = new QWidget(inputWidget);
    widget_4->setObjectName(QStringLiteral("widget_4"));
    widget_4->setMaximumSize(QSize(193, 28));

    QHBoxLayout * horizontalLayout = new QHBoxLayout(widget_4);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    horizontalLayout->setContentsMargins(1, 1, 1, 1);

    rememberPassord = new QCheckBox();
    rememberPassord->setObjectName(QStringLiteral("rememberPassord"));
    rememberPassord->setText(QObject::tr("Remember password"));

    autoLogin = new QCheckBox();
    autoLogin->setText(QObject::tr("Auto login"));

    QSpacerItem * horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addWidget(rememberPassord);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(autoLogin);

    inputGridLayout->addWidget(userList, 0, 1, 1, 1);
    inputGridLayout->addWidget(password, 1, 1, 1, 1);
    inputGridLayout->addWidget(widget_4, 2, 1, 1, 1);

    inputGridLayout->addWidget(forgetPassword, 1, 2, 1, 1);
    inputGridLayout->addWidget(regist, 0, 2, 1, 1);

    inputGridLayout->addWidget(login, 3, 1, 1, 1);

    inputWidget->setLayout(inputGridLayout);

    QHBoxLayout * bottomLayout = new QHBoxLayout();
    bottomLayout->addSpacerItem(new QSpacerItem(50,50,QSizePolicy::Expanding,QSizePolicy::Expanding));
    bottomLayout->addWidget(iconWidget);
    bottomLayout->addWidget(inputWidget);
    bottomLayout->addSpacerItem(new QSpacerItem(50,50,QSizePolicy::Expanding,QSizePolicy::Expanding));

    bottomWidget->setLayout(bottomLayout);

    QObject::connect(userList,SIGNAL(currentTextChanged(QString)),q_ptr,SLOT(validateInput(QString)));
    QObject::connect(login,SIGNAL(pressed()),q_ptr,SLOT(login()));
    QObject::connect(rememberPassord,SIGNAL(toggled(bool)),q_ptr,SLOT(setPassword(bool)));
    QObject::connect(autoLogin,SIGNAL(clicked(bool)),rememberPassord,SLOT(setChecked(bool)));
    QObject::connect(password,SIGNAL(textEdited(QString)),q_ptr,SLOT(validateInput(QString)));

    mainDialog = NULL;

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

    contentLayout->addWidget(imageWidget);
    contentLayout->addWidget(bottomWidget);

    contentLayout->setStretchFactor(imageWidget,9);
    contentLayout->setStretchFactor(bottomWidget,7);

    contentWidget->setLayout(contentLayout);

    q_ptr->setContentWidget(contentWidget);
    userList->lineEdit()->installEventFilter(q_ptr);
    password->installEventFilter(q_ptr);
}

int LoginDialogPrivate::userIndex(QString text)
{
    for(int i = 0; i < localUserInfo.size(); i++)
    {
        if(localUserInfo.at(i)->accountId == text)
        {
            return i;
        }
    }

    return -1;
}

LoginDialog::LoginDialog(QWidget *parent) :
    Widget(parent),
    d_ptr(new LoginDialogPrivate(this))
{
    RSingleton<Subject>::instance()->attach(this);
    //在多屏状态下，默认选择第一个屏幕
    QSize size = qApp->desktop()->screen()->size();

    setFixedSize(Constant::LOGIN_FIX_WIDTH,Constant::LOGIN_FIX_HEIGHT);
    setGeometry((size.width() - Constant::LOGIN_FIX_WIDTH)/2,(size.height() - Constant::LOGIN_FIX_HEIGHT)/2,Constant::LOGIN_FIX_WIDTH,Constant::LOGIN_FIX_HEIGHT);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    createTrayMenu();
    loadLocalSettings();

    connect(NetConnector::instance(),SIGNAL(connected(bool)),this,SLOT(respConnect(bool)));
    connect(MessDiapatch::instance(),SIGNAL(recvLoginResponse(ResponseLogin,LoginResponse)),this,SLOT(recvLoginResponse(ResponseLogin,LoginResponse)));
    connect(MessDiapatch::instance(),SIGNAL(recvFriendRequest(OperateFriendResponse)),this,SLOT(recvFriendResponse(OperateFriendResponse)));
    connect(MessDiapatch::instance(),SIGNAL(recvText(TextResponse)),this,SLOT(procRecvText(TextResponse)));
    connect(MessDiapatch::instance(),SIGNAL(recvUserStateChangedResponse(MsgOperateResponse,UserStateResponse)),
            this,SLOT(recvUserStateChanged(MsgOperateResponse,UserStateResponse)));
    QTimer::singleShot(0, this, SLOT(readLocalUser()));
}

void LoginDialog::respConnect(bool flag)
{
    MQ_D(LoginDialog);
    if(flag)
    {
#ifndef __NO_SERVER__
        LoginRequest * request = new LoginRequest();
        request->accountId = d->userList->currentText();
        request->password = RUtil::MD5( d->password->text());
        request->status = d->onlineState->state();
        G_OnlineStatus = d->onlineState->state();
        RSingleton<MsgWrap>::instance()->handleMsg(request);
#else
        recvLoginResponse(LOGIN_SUCCESS,LoginResponse());
#endif
    }
    else
    {
        RLOG_ERROR("Connect to server %s:%d error!",G_ServerIp.toLocal8Bit().data(),G_ServerPort);
        RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Connect to server error!"),RMessageBox::Yes);
    }
}

void LoginDialog::login()
{
#ifndef __NO_SERVER__
    NetConnector::instance()->connect();
#else
    LoginResponse response;
    recvLoginResponse(LOGIN_SUCCESS, response);
#endif
}

void LoginDialog::minsize()
{
    MQ_D(LoginDialog);
    if(d->trayIcon)
    {
        if(!d->trayIcon->isVisible())
        {
            d->trayIcon->setVisible(true);
        }
    }

    this->setVisible(false);
}

void LoginDialog::closeWindow()
{
    qDebug()<<__LINE__<<__FUNCTION__;
    RSingleton<TaskManager>::instance()->removeAll();
    this->close();
}

void LoginDialog::setPassword(bool flag)
{
    MQ_D(LoginDialog);
    if(!flag && d->autoLogin->isChecked())
    {
        d->autoLogin->setChecked(false);
    }
}

void LoginDialog::createTrayMenu()
{
    MQ_D(LoginDialog);
    d->trayIcon = new SystemTrayIcon();
    d->trayIcon->setModel(SystemTrayIcon::System_Login);
    d->trayIcon->setVisible(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,true).toBool());

    connect(d->trayIcon,SIGNAL(quitApp()),this,SLOT(closeWindow()));
    connect(d->trayIcon,SIGNAL(showMainPanel()),this,SLOT(showNormal()));
}

void LoginDialog::loadLocalSettings()
{
    G_ServerIp = RUtil::globalSettings()->value(Constant::SETTING_NETWORK_IP,Constant::DEFAULT_NETWORK_IP).toString();
    G_ServerPort = RUtil::globalSettings()->value(Constant::SETTING_NETWORK_PORT,Constant::DEFAULT_NETWORK_PORT).toUInt();
}

int LoginDialog::isContainUser()
{
    MQ_D(LoginDialog);
    qDebug()<<d->userList->currentText()<<"===hha";
    return d->userIndex(d->userList->currentText());
}

void LoginDialog::resetDefaultInput()
{
    MQ_D(LoginDialog);

    d->isSystemUserIcon = true;
    QString fullPath = RSingleton<ImageManager>::instance()->getSystemUserIcon();
    QFileInfo  info(fullPath);
    d->defaultUserIconPath = info.fileName();

    d->password->setText(tr(""));
    d->autoLogin->setChecked(false);
    d->rememberPassord->setChecked(false);

    d->onlineState->setState(ProtocolType::STATUS_ONLINE);
    d->userIcon->setPixmap(fullPath);

    d->login->setEnabled(false);
    d->login->style()->unpolish(d->login);
    d->login->style()->polish(d->login);
}

void LoginDialog::readLocalUser()
{
    MQ_D(LoginDialog);

    RSingleton<UserInfoFile>::instance()->readUsers(d->localUserInfo);

    if(d->localUserInfo.size() > 0)
    {
        respItemChanged(d->localUserInfo.first()->accountId);
        foreach(UserInfoDesc * user,d->localUserInfo)
        {
            ComboxItem * item = new ComboxItem;
            connect(item,SIGNAL(deleteItem(QString)),this,SLOT(removeUserItem(QString)));
            connect(item,SIGNAL(itemClicked(QString)),this,SLOT(respItemChanged(QString)));
            item->setNickName(user->userName);
            item->setAccountId(user->accountId);

            QListWidgetItem* widgetItem = new QListWidgetItem(d->userListWidget);
            d->userListWidget->setItemWidget(widgetItem,item);
        }
    }
    else
    {
        resetDefaultInput();
    }
}

void LoginDialog::validateInput(QString /*text*/)
{
    MQ_D(LoginDialog);
    if(d->userList->currentText().size() >0 && d->password->text().size() > 0)
    {
        if(d->numberExp.exactMatch(d->userList->currentText()) && d->passExp.exactMatch(d->password->text()))
        {
            d->login->setEnabled(true);
            repolish(d->login);
            return;
        }
    }

    d->login->setEnabled(false);

    repolish(d->login);
}

void LoginDialog::showNetSettings()
{
    NetSettings * settings = new NetSettings(this);
    settings->initLocalSettings();
    settings->show();
}

void LoginDialog::removeUserItem(QString accountId)
{
    MQ_D(LoginDialog);

    int index = -1;
    for(int i = 0; i < d->userListWidget->count();i++)
    {
        QListWidgetItem * item = d->userListWidget->item(i);
        ComboxItem * comboxItem = dynamic_cast<ComboxItem *>(d->userListWidget->itemWidget(item));
        if(comboxItem && comboxItem->getAccountId() == accountId)
        {
            index = i;
            break;
        }
    }

    if(index >= 0)
    {
       int result = RMessageBox::warning(this,"warning",QString("Delete count %1 from list?").arg(accountId),RMessageBox::Yes|RMessageBox::No,RMessageBox::No);
       if(result == RMessageBox::Yes)
       {
            d->userListWidget->takeItem(index);
            QList<UserInfoDesc *>::iterator iter = d->localUserInfo.begin();
            while(iter != d->localUserInfo.end())
            {
                if((*iter)->accountId == accountId)
                {
                    d->localUserInfo.erase(iter);
                    break;
                }
                iter++;
            }
            RSingleton<UserInfoFile>::instance()->saveUsers(d->localUserInfo);
       }
    }
}

void LoginDialog::respItemChanged(QString id)
{
    MQ_D(LoginDialog);

    bool existed = false;
    QList<UserInfoDesc *>::iterator iter = d->localUserInfo.begin();
    while(iter != d->localUserInfo.end())
    {
        if((*iter)->accountId == id)
        {
            UserInfoDesc * userInfo = *iter;
            d->userList->hidePopup();
            d->userList->setEditText(id);
            d->password->setText(userInfo->originPassWord);
            d->rememberPassord->setChecked(userInfo->isRemberPassword);
            d->autoLogin->setChecked(userInfo->isAutoLogin);
            if(userInfo->isSystemPixMap)
            {
                QPixmap pixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon(userInfo->pixmap));
                if(pixmap.isNull())
                {
                    userInfo->pixmap = d->defaultUserIconPath;
                    d->userIcon->setPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon(userInfo->pixmap));
                }
                else
                {
                    d->userIcon->setPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon(userInfo->pixmap));
                }
            }

            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<userInfo->loginState;

            d->onlineState->setState((OnlineStatus)userInfo->loginState);
            d->isNewUser = false;

            validateInput("");

            existed = true;
            break;
        }
        iter++;
    }

    if(!existed)
    {
        d->isNewUser = true;
    }
}

void LoginDialog::showRegistDialog()
{
    disconnect(NetConnector::instance(),SIGNAL(connected(bool)),this,SLOT(respConnect(bool)));

    RegistDialog * dialog = new RegistDialog(this);
    connect(dialog,SIGNAL(destroyed(QObject*)),this,SLOT(respRegistDialogDestory(QObject*)));
    dialog->show();
}

void LoginDialog::respRegistDialogDestory(QObject *)
{
    connect(NetConnector::instance(),SIGNAL(connected(bool)),this,SLOT(respConnect(bool)));
}

void LoginDialog::recvLoginResponse(ResponseLogin status, LoginResponse response)
{
    MQ_D(LoginDialog);
    if(status == LOGIN_SUCCESS)
    {
        int index = -1;
        if( (index = isContainUser()) >= 0)
        {
            if(index < d->localUserInfo.size())
            {
                d->localUserInfo.at(index)->userName = response.baseInfo.nickName;
                d->localUserInfo.at(index)->accountId = response.baseInfo.accountId;
                d->localUserInfo.at(index)->originPassWord = d->password->text();
                d->localUserInfo.at(index)->password = RUtil::MD5(d->password->text());
                d->localUserInfo.at(index)->isAutoLogin =  d->autoLogin->isChecked();
                d->localUserInfo.at(index)->isRemberPassword = d->rememberPassord->isChecked();
                d->localUserInfo.at(index)->loginState = (int)d->onlineState->state();
                RSingleton<UserInfoFile>::instance()->saveUsers(d->localUserInfo);
            }
        }
        else
        {
            UserInfoDesc * desc = new UserInfoDesc;
            desc->userName = response.baseInfo.nickName;
            desc->accountId = response.baseInfo.accountId;
            desc->loginState = (int)d->onlineState->state();
            desc->originPassWord = d->password->text();
            desc->password = RUtil::MD5(d->password->text());
            desc->isAutoLogin = d->autoLogin->isChecked();
            desc->isRemberPassword = d->rememberPassord->isChecked();
            desc->isSystemPixMap = d->isSystemUserIcon;
            desc->pixmap = d->defaultUserIconPath;

            d->localUserInfo.append(desc);
            RSingleton<UserInfoFile>::instance()->saveUsers(d->localUserInfo);
        }

        G_UserBaseInfo = response.baseInfo;

        QString apppath = qApp->applicationDirPath() + QString(Constant::PATH_UserPath);
        G_Temp_Picture_Path = QString("%1/%2/%3").arg(apppath).arg(G_UserBaseInfo.accountId).arg(Constant::UserTempName);
        RUtil::createDir(G_Temp_Picture_Path);

        setVisible(false);
        d->trayIcon->disconnect();
        d->trayIcon->setModel(SystemTrayIcon::System_Main);

        if(!d->mainDialog)
        {
            d->mainDialog = new MainDialog();
        }

        if(!d->notifyWindow)
        {
            d->notifyWindow = new NotifyWindow;
            connect(d->notifyWindow,SIGNAL(showSystemNotifyInfo(NotifyInfo,int)),this,SLOT(viewSystemNotify(NotifyInfo,int)));
        }

        d->mainDialog->show();
        d->mainDialog->setLogInState(d->onlineState->state());

        FriendListRequest * request = new FriendListRequest;
        request->accountId = G_UserBaseInfo.accountId;
        RSingleton<MsgWrap>::instance()->handleMsg(request);
    }
    else
    {
        QString errorInfo;
        switch(status)
        {
            case LOGIN_UNREGISTERED:
                 errorInfo = QObject::tr("User not registered");
                 break;
            case LOGIN_PASS_ERROR:
                 errorInfo = QObject::tr("Incorrect password");
                 break;
            case LOGIN_SERVER_REFUSED:
                 errorInfo = QObject::tr("Server Unreachable");
                 break;
            case LOGIN_USER_LOGINED:
                errorInfo = QObject::tr("Account has sign in");
                break;
            case LOGIN_SERVER_NOT_RESP:
                 errorInfo = QObject::tr("");
                 break;
            case LOGIN_FAILED:
            default:
                 errorInfo = QObject::tr("Login Failed");
                 break;
        }

        RMessageBox::warning(this,"Warning",errorInfo,RMessageBox::Yes);
    }
}

/*!
 * @brief 接收好友操作响应信息
 * @details 可操作好友请求、同意请求、拒绝请求
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void LoginDialog::recvFriendResponse(OperateFriendResponse resp)
{
    MQ_D(LoginDialog);

    RSingleton<MediaPlayer>::instance()->play(MediaPlayer::MediaSystem);

    NotifyInfo  info;
    info.identityId = RUtil::UUID();
    info.accountId = resp.requestInfo.accountId;
    info.nickName = resp.requestInfo.nickName;
    info.face = resp.requestInfo.face;
    info.type = NotifySystem;
    info.stype = resp.stype;
    info.pixmap = RSingleton<ImageManager>::instance()->getIcon(ImageManager::ICON_SYSTEMNOTIFY,ImageManager::ICON_64);
    info.ofriendResult = resp.result;

    d->notifyWindow->addNotifyInfo(info);
    d->notifyWindow->showMe();

    d->trayIcon->notify(SystemTrayIcon::SystemNotify,info.identityId);
}

//TODO 考虑若未打开窗口，消息如何存储？？
/*!
     * @brief 接收发送的聊天消息
     * @details 若聊天对象的窗口未创建，则使用系统通知，进行提示；
     *          若聊天对象的窗口已创建，但不可见，则将信息保存并将窗口设置可见
     * @param[in] response 消息体内容
     * @return 是否插入成功
     */
void LoginDialog::procRecvText(TextResponse response)
{
    MQ_D(LoginDialog);
    UserClient * client = RSingleton<UserManager>::instance()->client(response.fromAccountId);
    if(client)
    {
        //【1】存储消息至数据库
        SimpleUserInfo userInfo;
        qDebug()<<response.sendData;
        userInfo.accountId = "0";
        ChatInfoUnit unit = RSingleton<JsonResolver>::instance()->ReadJSONFile(response.sendData.toLocal8Bit());
        SQLProcess::instance()->insertTableUserChatInfo(DatabaseManager::Instance()->getLastDB(),unit,userInfo);

        //【2】判断窗口是否创建或者是否可见
        if(client->chatWidget && client->chatWidget->isVisible())
        {
            if(response.msgCommand == MSG_TEXT_TEXT)
            {
                client->chatWidget->showRecentlyChatMsg();
            }
            else if(response.msgCommand == MSG_TEXT_SHAKE)
            {
                client->chatWidget->shakeWindow();
            }
        }
        else
        {
            if(response.msgCommand == MSG_TEXT_SHAKE)
            {
                if(!client->chatWidget)
                {
                    client->chatWidget = new AbstractChatWidget();
                    client->chatWidget->setUserInfo(client->simpleUserInfo);
                    client->chatWidget->initChatRecord();
                }
                client->chatWidget->show();
            }
            else if(response.msgCommand == MSG_TEXT_TEXT)
            {
                //TODO 未将文本消息设置到提示框中
                NotifyInfo  info;
                info.identityId = RUtil::UUID();
                info.msgCommand = response.msgCommand;
                info.accountId = response.fromAccountId;
                info.nickName = client->simpleUserInfo.nickName;
                info.type = NotifyUser;
                info.stype = response.type;
                info.face = client->simpleUserInfo.face;
                info.pixmap = RSingleton<ImageManager>::instance()->getSystemUserIcon();

                QString inserInfoId = d->notifyWindow->addNotifyInfo(info);
                d->notifyWindow->showMe();
                if(inserInfoId == info.identityId)
                {
                    d->trayIcon->notify(SystemTrayIcon::UserNotify,info.identityId,RSingleton<ImageManager>::instance()->getSystemUserIcon());
                }
                else
                {
                    d->trayIcon->frontNotify(SystemTrayIcon::UserNotify,inserInfoId);
                }
            }
        }

        if(response.msgCommand == MSG_TEXT_TEXT)
        {
            RSingleton<MediaPlayer>::instance()->play(MediaPlayer::MediaMsg);
        }
        else if(response.msgCommand == MSG_TEXT_SHAKE)
        {
            RSingleton<MediaPlayer>::instance()->play(MediaPlayer::MediaShake);
        }
    }
}

void LoginDialog::recvUserStateChanged(MsgOperateResponse result, UserStateResponse response)
{
    if(result == STATUS_SUCCESS && response.accountId != G_UserBaseInfo.accountId)
    {
        UserClient * client = RSingleton<UserManager>::instance()->client(response.accountId);
        if(client)
        {
            client->toolItem->setStatus(response.onStatus);
            if(response.onStatus != STATUS_OFFLINE && response.onStatus != STATUS_HIDE)
            {
                RSingleton<MediaPlayer>::instance()->play(MediaPlayer::MediaOnline);
            }
        }
    }
}

/*!
 * @brief 查看系统通知消息
 * @param[in] info 通知消息内容
 * @return 无
 */
void LoginDialog::viewSystemNotify(NotifyInfo info,int notifyCount)
{
    MQ_D(LoginDialog);

    if(info.type == NotifySystem)
    {
        ResponseFriendApply reqType = (ResponseFriendApply)info.ofriendResult;

        SystemNotifyView * view = new SystemNotifyView();
        connect(view,SIGNAL(chatWidth(QString)),this,SLOT(openChatDialog(QString)));

        view->setNotifyType(reqType);
        view->setNotifyInfo(info);
        view->show();
    }
    else if(info.type == NotifyUser)
    {
        UserClient * client = RSingleton<UserManager>::instance()->client(info.accountId);
        if(client)
        {
            if(client->chatWidget == NULL)
            {
                client->chatWidget = new AbstractChatWidget();
                client->chatWidget->setUserInfo(client->simpleUserInfo);
                client->chatWidget->initChatRecord();
            }

//            client->chatWidget->showRecentlyChatMsg(notifyCount);
            client->chatWidget->show();
        }
    }

    d->trayIcon->removeNotify(info.identityId);
}

void LoginDialog::openChatDialog(QString accountId)
{
   UserClient * client = RSingleton<UserManager>::instance()->client(accountId);
   if(client->chatWidget == NULL)
   {
       client->chatWidget = new AbstractChatWidget();
       client->chatWidget->setUserInfo(client->simpleUserInfo);
       client->chatWidget->initChatRecord();
   }

   client->chatWidget->show();
}

LoginDialog::~LoginDialog()
{
    MQ_D(LoginDialog);

    RSingleton<Subject>::instance()->detach(this);

    if(d->toolBar)
    {
        delete d->toolBar;
        d->toolBar = NULL;
    }

    if(d->mainDialog)
    {
        delete d->mainDialog;
        d->mainDialog = NULL;
    }

    if(d->trayIcon)
    {
       delete d->trayIcon;
       d->trayIcon = NULL;
    }
}

void LoginDialog::onMessage(MessageType type)
{
    MQ_D(LoginDialog);
    switch(type)
    {
        case MESS_SETTINGS:
            {
                d->trayIcon->setVisible(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,true).toBool());
                break;
            }
        case MESS_NOTIFY_WINDOWS:
            {
                d->notifyWindow->showMe();
                break;
            }
        default:
                break;
    }
}

void LoginDialog::resizeEvent(QResizeEvent *)
{
    MQ_D(LoginDialog);

    d->toolBar->setGeometry(WINDOW_MARGIN_SIZE,0,width() - WINDOW_MARGIN_SIZE * 3,d->toolBar->size().height());

    QRect ioconRect = d->userIcon->geometry();
    d->onlineState->setGeometry(ioconRect.right() - 20,ioconRect.bottom() - 22,30,30);
}

void LoginDialog::closeEvent(QCloseEvent *)
{
    qApp->exit();
}

bool LoginDialog::eventFilter(QObject *obj, QEvent *event)
{
    MQ_D(LoginDialog);

    if(event->type() == QEvent::KeyPress)
    {
        if(obj == d->password)
        {
            QKeyEvent * keyEvent = dynamic_cast<QKeyEvent *>(event);
            if(!d->isNewUser && keyEvent->key() == Qt::Key_Backspace)
            {
                d->password->setText("");
            }
        }
        //Yang 20171213待实现对输入框键盘事件的捕捉
        else if(obj == d->userList->lineEdit())
        {

        }
    }

    return Widget::eventFilter(obj,event);
}

ComboxItem::ComboxItem(QWidget *parent):QWidget(parent)
{
    QWidget * widget = new QWidget(this);

    iconLabel = new RIconLabel;
    iconLabel->setTransparency(true);
    iconLabel->setEnterCursorChanged(false);
    iconLabel->setEnterHighlight(false);
    iconLabel->setCorner(true);

    iconLabel->setPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon());

    nickNameLabel = new QLabel(widget);
    nickNameLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    nickNameLabel->setAlignment(Qt::AlignLeft);
    nickNameLabel->setMinimumWidth(125);
    nickNameLabel->setMinimumHeight(22);

    accountLabel = new QLabel(widget);
    accountLabel->setMinimumHeight(22);
    accountLabel->setAlignment(Qt::AlignLeft);

    closeButt = new QPushButton;
    closeButt->setText("X");
    closeButt->setFixedSize(20,20);
    closeButt->setToolTip(QObject::tr("Remove account"));
    connect(closeButt,SIGNAL(pressed()),this,SLOT(prepareDelete()));

    QGridLayout * gridlayout = new QGridLayout;
    gridlayout->setContentsMargins(2,2,2,2);
    gridlayout->addWidget(iconLabel,0,0,2,1);
    gridlayout->addWidget(nickNameLabel,0,1,1,6);
    gridlayout->addWidget(accountLabel,1,1,1,6);
    gridlayout->addWidget(closeButt,0,8,2,1);
    gridlayout->setHorizontalSpacing(1);
    gridlayout->setVerticalSpacing(1);

    widget->setLayout(gridlayout);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(widget);
    setLayout(layout);
}

ComboxItem::~ComboxItem()
{

}

void ComboxItem::setNickName(QString name)
{
    nickNameLabel->setText(name);
}

void ComboxItem::setAccountId(QString id)
{
    accountLabel->setText(id);
}

QString ComboxItem::getAccountId()
{
    return accountLabel->text();
}

void ComboxItem::mouseReleaseEvent(QMouseEvent *)
{
    emit itemClicked(accountLabel->text());
}

void ComboxItem::prepareDelete()
{
    emit deleteItem(accountLabel->text());
}
