#include "logindialog.h"
#include "ui_logindialog.h"

#include <QList>
#include <QToolButton>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>
#include <QListView>
#include <QMenu>
#include <QAction>
#include <QDesktopWidget>

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
#include "msgcontext.h"

class LoginDialogPrivate : public QObject,public GlobalData<LoginDialog>
{
    Q_DECLARE_PUBLIC(LoginDialog)
public:
    LoginDialogPrivate()
    {
        numberExp.setPattern("[1-9]\\d{6}");
        passExp.setPattern("\\w{6,16}");

        isSystemUserIcon = true;
        isNewUser = true;
    }
    ~LoginDialogPrivate(){}

    int userIndex(QString text);

    QList<UserInfoDesc *> localUserInfo;

    QPoint mousePreseePoint;

    bool isSystemUserIcon;
    QString defaultUserIconPath;

    bool isNewUser;

    QRegExp numberExp;
    QRegExp passExp;
};

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
    QDialog(parent),
    m_actionManager(new ActionManager(this)),
    d_ptr(new LoginDialogPrivate()),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    RSingleton<Subject>::instance()->attach(this);

    initWidget();
    createTrayMenu();
    loadLocalSettings();
}

void LoginDialog::login()
{
    MQ_D(LoginDialog);

    int index = -1;

    if(!RSingleton<NetConnector>::instance()->connect())
    {
        RLOG_ERROR("Connect to server %s:%d error!",G_ServerIp.toLocal8Bit().data(),G_ServerPort);
        RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Connect to server error!"),RMessageBox::Yes);
        return;
    }

    LoginRequest * request = new LoginRequest();
    request->accountId = ui->userList->currentText();
    request->password =  ui->password->text();
    request->status = STATUS_ONLINE;
    RSingleton<MsgWrap>::instance()->handleMsg(request);

    //Yang 20171214待将此处加入网络
    if( (index = isContainUser()) >= 0)
    {
        if(index < d->localUserInfo.size())
        {
            d->localUserInfo.at(index)->userName = ui->userList->currentText();
            d->localUserInfo.at(index)->originPassWord = ui->password->text();
            d->localUserInfo.at(index)->password = RUtil::MD5(ui->password->text());
            d->localUserInfo.at(index)->isAutoLogin =  ui->autoLogin->isChecked();
            d->localUserInfo.at(index)->isRemberPassword = ui->rememberPassord->isChecked();
            RSingleton<UserInfoFile>::instance()->saveUsers(d->localUserInfo);
        }
    }
    else
    {
        UserInfoDesc * desc = new UserInfoDesc;
        desc->userName = ui->userList->currentText();
        desc->loginState = (int)onlineState->state();
        desc->originPassWord = ui->password->text();
        desc->password = RUtil::MD5(ui->password->text());
        desc->isAutoLogin = ui->autoLogin->isChecked();
        desc->isRemberPassword = ui->rememberPassord->isChecked();
        desc->isSystemPixMap = d->isSystemUserIcon;
        desc->pixmap = d->defaultUserIconPath;

        d->localUserInfo.append(desc);
        RSingleton<UserInfoFile>::instance()->saveUsers(d->localUserInfo);
    }

    this->setVisible(false);
    trayIcon->disconnect();
    trayIcon->setModel(SystemTrayIcon::System_Main);

    if(!mainDialog)
    {
        mainDialog = new MainDialog();
    }

    mainDialog->show();
}

void LoginDialog::minsize()
{
    if(trayIcon)
    {
        if(!trayIcon->isVisible())
        {
            trayIcon->setVisible(true);
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

void LoginDialog::switchUser(int index)
{
    MQ_D(LoginDialog);

    if(d->localUserInfo.size() > 0 && index >= 0 && index < d->localUserInfo.size() )
    {
        UserInfoDesc * userInfo = d->localUserInfo.at(index);
        ui->password->setText(userInfo->originPassWord);
        ui->rememberPassord->setChecked(userInfo->isRemberPassword);
        ui->autoLogin->setChecked(userInfo->isAutoLogin);
        if(userInfo->isSystemPixMap)
        {
            QPixmap pixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon(userInfo->pixmap));
            if(pixmap.isNull())
            {
                userInfo->pixmap = d->defaultUserIconPath;
                ui->userIcon->setPixmap(QPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon(userInfo->pixmap)).scaled(ui->userIcon->width(),ui->userIcon->height()));
            }
            else
            {
                ui->userIcon->setPixmap(pixmap.scaled(ui->userIcon->width(),ui->userIcon->height()));
            }
        }

        onlineState->setState((OnLineState::UserState)userInfo->loginState);
        d->isNewUser = false;

        validateInput("");
    }
    else
    {
        d->isNewUser = true;
    }
}

void LoginDialog::initWidget()
{
    MQ_D(LoginDialog);

    //在多屏状态下，默认选择第一个屏幕
    QSize size = qApp->desktop()->screen()->size();

    this->setGeometry((size.width() - Constant::LOGIN_FIX_WIDTH)/2,(size.height() - Constant::LOGIN_FIX_HEIGHT)/2,Constant::LOGIN_FIX_WIDTH,Constant::LOGIN_FIX_HEIGHT);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    mainDialog = NULL;

    toolBar = new ToolBar(this);

    RToolButton * systemSetting = ActionManager::instance()->createToolButton(Id(Constant::TOOL_SETTING),this,SLOT(showNetSettings()));
    systemSetting->setToolTip(tr("System Setting"));

    RToolButton * minSize = ActionManager::instance()->createToolButton(Id(Constant::TOOL_MIN),this,SLOT(minsize()));
    minSize->setToolTip(tr("Min"));

    RToolButton * closeButt = ActionManager::instance()->createToolButton(Id(Constant::TOOL_CLOSE),this,SLOT(closeWindow()));
    closeButt->setToolTip(tr("Close"));

    toolBar->addStretch(1);
    toolBar->appendToolButton(systemSetting);
    toolBar->appendToolButton(minSize);
    toolBar->appendToolButton(closeButt);

    ui->userList->setEditable(true);

    QRegExpValidator * numberValidator = new QRegExpValidator(d->numberExp);
    ui->userList->lineEdit()->setValidator(numberValidator);
    ui->userList->lineEdit()->setPlaceholderText(tr("Input number"));
    ui->userList->setView(new QListView());
    ui->userList->lineEdit()->installEventFilter(this);

    QRegExpValidator * passValidator = new QRegExpValidator(d->passExp);
    ui->password->setValidator(passValidator);
    ui->password->setPlaceholderText(tr("Input password"));
    ui->password->setEchoMode(QLineEdit::Password);
    ui->password->installEventFilter(this);

    ui->rememberPassord->setText(tr("Remember password"));
    ui->autoLogin->setText(tr("Auto login"));
    ui->login->setText(tr("Sin in"));
    ui->forgetPassword->setText(tr("Forget Password"));
    ui->regist->setText(tr("Sin up"));

    ui->regist->installEventFilter(this);

    connect(ui->userList,SIGNAL(currentIndexChanged(int)),this,SLOT(switchUser(int)));
    connect(ui->login,SIGNAL(pressed()),this,SLOT(login()));
    connect(ui->autoLogin,SIGNAL(clicked(bool)),ui->rememberPassord,SLOT(setChecked(bool)));
    connect(ui->password,SIGNAL(textEdited(QString)),this,SLOT(validateInput(QString)));
    connect(ui->userList,SIGNAL(currentTextChanged(QString)),this,SLOT(validateInput(QString)));

    onlineState = new OnLineState(this);

    QTimer::singleShot(0, this, SLOT(readLocalUser()));
}

void LoginDialog::createTrayMenu()
{
    trayIcon = new SystemTrayIcon();
    trayIcon->setModel(SystemTrayIcon::System_Login);
    trayIcon->setVisible(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,true).toBool());

    connect(trayIcon,SIGNAL(quitApp()),this,SLOT(closeWindow()));
    connect(trayIcon,SIGNAL(showMainPanel()),this,SLOT(showNormal()));
}

void LoginDialog::loadLocalSettings()
{
    G_ServerIp = RUtil::globalSettings()->value(Constant::SETTING_NETWORK_IP,Constant::DEFAULT_NETWORK_IP).toString();
    G_ServerPort = RUtil::globalSettings()->value(Constant::SETTING_NETWORK_PORT,Constant::DEFAULT_NETWORK_PORT).toUInt();
}

int LoginDialog::isContainUser()
{
    MQ_D(LoginDialog);
    return d->userIndex(ui->userList->currentText());
}

void LoginDialog::resetDefaultInput()
{
    MQ_D(LoginDialog);

    d->isSystemUserIcon = true;
    QString fullPath = RSingleton<ImageManager>::instance()->getSystemUserIcon();
    QFileInfo  info(fullPath);
    d->defaultUserIconPath = info.fileName();

    ui->password->setText(tr(""));
    ui->autoLogin->setChecked(false);
    ui->rememberPassord->setChecked(false);
    QPixmap pixmap(fullPath);

    onlineState->setState(OnLineState::STATE_ONLINE);
    ui->userIcon->setPixmap(pixmap.scaled(ui->userIcon->width(),ui->userIcon->height()));

    ui->login->setEnabled(false);
    ui->login->style()->unpolish(ui->login);
    ui->login->style()->polish(ui->login);
}

void LoginDialog::readLocalUser()
{
    MQ_D(LoginDialog);

    RSingleton<UserInfoFile>::instance()->readUsers(d->localUserInfo);

    if(d->localUserInfo.size() > 0)
    {
        foreach(UserInfoDesc * user,d->localUserInfo)
        {
             ui->userList->addItem(user->userName);
        }
    }
    else
    {
        resetDefaultInput();
    }

    QPoint point = ui->userIcon->mapTo(this,QPoint(ui->userIcon->size().width(),ui->userIcon->size().height()));

    onlineState->setGeometry(point.x() - Constant::TOOL_WIDTH,point.y() - Constant::TOOL_HEIGHT,Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);
}

void LoginDialog::validateInput(QString /*text*/)
{
    MQ_D(LoginDialog);
    if(ui->userList->currentText().size() >0 && ui->password->text().size() > 0)
    {
        if(d->numberExp.exactMatch(ui->userList->currentText()) && d->passExp.exactMatch(ui->password->text()))
        {
            ui->login->setEnabled(true);
            ui->login->style()->unpolish(ui->login);
            ui->login->style()->polish(ui->login);
            return;
        }
    }

    ui->login->setEnabled(false);
    ui->login->style()->unpolish(ui->login);
    ui->login->style()->polish(ui->login);
}

void LoginDialog::showNetSettings()
{
    NetSettings * settings = new NetSettings(this);
    settings->initLocalSettings();
    settings->show();
}

LoginDialog::~LoginDialog()
{
    if(toolBar)
    {
        delete toolBar;
        toolBar = NULL;
    }

    if(mainDialog)
    {
        delete mainDialog;
        mainDialog = NULL;
    }

    if(trayIcon)
    {
       delete trayIcon;
       trayIcon = NULL;
    }

    delete ui;
}

void LoginDialog::onMessage(MessageType type)
{
    switch(type)
    {
        case MESS_SETTINGS:
                            {
                                trayIcon->setVisible(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,true).toBool());
                                break;
                            }
        default:
                break;
    }
}

void LoginDialog::resizeEvent(QResizeEvent *event)
{
    toolBar->setGeometry(event->size().width() - toolBar->size().width(),0,toolBar->size().width(),toolBar->size().height());
}

void LoginDialog::mousePressEvent(QMouseEvent *event)
{
    MQ_D(LoginDialog);
    d->mousePreseePoint = event->pos();
}

void LoginDialog::mouseMoveEvent(QMouseEvent *event)
{
    MQ_D(LoginDialog);

    QPoint tmpPoint = event->pos() - d->mousePreseePoint;

    this->setGeometry(this->x() + tmpPoint.x(),this->y() + tmpPoint.y(),Constant::LOGIN_FIX_WIDTH,Constant::LOGIN_FIX_HEIGHT);
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
        if(obj == ui->password)
        {
            QKeyEvent * keyEvent = dynamic_cast<QKeyEvent *>(event);
            if(!d->isNewUser && keyEvent->key() == Qt::Key_Backspace)
            {
                ui->password->setText("");
            }
        }
        //Yang 20171213待实现对输入框键盘事件的捕捉
        else if(obj == ui->userList->lineEdit())
        {

        }
    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        if(obj == ui->regist)
        {
            RegistDialog * dialog = new RegistDialog(this);
            dialog->show();
        }
    }
    return QDialog::eventFilter(obj,event);
}
