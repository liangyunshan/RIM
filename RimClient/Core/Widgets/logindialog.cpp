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
#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QApplication>
#include <QDir>

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
    QComboBox *userList;

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
    userList = new QComboBox();
    userList->setFixedSize(QSize(193, 28));
    userList->setEditable(true);
    userList->lineEdit()->setValidator(numberValidator);
    userList->lineEdit()->setPlaceholderText(QObject::tr("Input number"));
    userList->setView(new QListView());

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

    QObject::connect(userList,SIGNAL(currentIndexChanged(int)),q_ptr,SLOT(switchUser(int)));
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

    userList->setEditable(true);

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
        if(localUserInfo.at(i)->userName == text)
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

    QTimer::singleShot(0, this, SLOT(readLocalUser()));
}

void LoginDialog::respConnect(bool flag)
{
    MQ_D(LoginDialog);

    if(flag)
    {
        LoginRequest * request = new LoginRequest();
        request->accountId = d->userList->currentText();
        request->password = RUtil::MD5( d->password->text());
        request->status = STATUS_ONLINE;
        RSingleton<MsgWrap>::instance()->handleMsg(request);
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

void LoginDialog::switchUser(int index)
{
    MQ_D(LoginDialog);

    if(d->localUserInfo.size() > 0 && index >= 0 && index < d->localUserInfo.size())
    {
        UserInfoDesc * userInfo = d->localUserInfo.at(index);
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

        d->onlineState->setState((OnLineState::UserState)userInfo->loginState);
        d->isNewUser = false;

        validateInput("");
    }
    else
    {
        d->isNewUser = true;
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

    d->onlineState->setState(OnLineState::STATE_ONLINE);
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
        foreach(UserInfoDesc * user,d->localUserInfo)
        {
             d->userList->addItem(user->userName);
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
            d->login->style()->unpolish(d->login);
            d->login->style()->polish(d->login);
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
                d->localUserInfo.at(index)->userName = d->userList->currentText();
                d->localUserInfo.at(index)->originPassWord = d->password->text();
                d->localUserInfo.at(index)->password = RUtil::MD5(d->password->text());
                d->localUserInfo.at(index)->isAutoLogin =  d->autoLogin->isChecked();
                d->localUserInfo.at(index)->isRemberPassword = d->rememberPassord->isChecked();
                RSingleton<UserInfoFile>::instance()->saveUsers(d->localUserInfo);
            }
        }
        else
        {
            UserInfoDesc * desc = new UserInfoDesc;
            desc->userName = d->userList->currentText();
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

        //shangchao 创建每个用户对应的缓存文件夹
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
//            d->notifyWindow->show();
        }

        d->mainDialog->show();
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
                                         errorInfo = QObject::tr("server Unreachable");
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

void LoginDialog::recvFriendResponse(OperateFriendResponse resp)
{
    MQ_D(LoginDialog);
    d->trayIcon->notify(SystemTrayIcon::SystemNotify);
    RSingleton<MediaPlayer>::instance()->play(MediaPlayer::MediaSystem);

    d->notifyWindow->showMe();

    NotifyInfo  info;
    info.accountId = resp.requestInfo.accountId;
    info.type = NotifySystem;
    info.pixmap = RSingleton<ImageManager>::instance()->getIcon(ImageManager::ICON_SYSTEMNOTIFY,ImageManager::ICON_64);
    d->notifyWindow->addNotifyInfo(info);

    ResponseFriendApply reqType = (ResponseFriendApply)resp.result;
    switch (reqType)
    {
        case FRIEND_REQUEST:
                            {
                                qDebug()<<resp.requestInfo.accountId<<"+++qingqiu";
                            }
                            break;
        case FRIEND_AGREE:
                            {
                                qDebug()<<resp.requestInfo.accountId<<"+++tongyi";
                            }
                            break;
        case FRIEND_REFUSE:
                            {
                                qDebug()<<resp.requestInfo.accountId<<"+++jujue";
                            }
                            break;
        default:
            break;
    }
}

LoginDialog::~LoginDialog()
{
    MQ_D(LoginDialog);
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
