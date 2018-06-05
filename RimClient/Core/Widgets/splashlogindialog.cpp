#include "splashlogindialog.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QPixmap>
#include <QDir>
#include <QApplication>
#include <QDateTime>
#include <QDebug>

#include "global.h"
#include "widget/rmessagebox.h"
#include "file/xmlparse.h"
#include "rsingleton.h"
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
#include "abstractchatwidget.h"

class SplashLoginDialogPrivate : public QObject,public GlobalData<SplashLoginDialog>
{
    Q_DECLARE_PUBLIC(SplashLoginDialog)
private:
    explicit SplashLoginDialogPrivate(SplashLoginDialog * q):q_ptr(q),
    trayIcon(nullptr),mainDialog(nullptr){

    }

    SplashLoginDialog * q_ptr;

    SystemTrayIcon * trayIcon;
    MainDialog * mainDialog;
};

SplashLoginDialog::SplashLoginDialog(QWidget *parent):
    QSplashScreen(parent),d_ptr(new SplashLoginDialogPrivate(this))
{
    RSingleton<Subject>::instance()->attach(this);
    setPixmap(QPixmap(":/background/resource/background/splash.png"));

    G_User = nullptr;

    QTimer::singleShot(0,this,SLOT(initResource()));
}

SplashLoginDialog::~SplashLoginDialog()
{

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

void SplashLoginDialog::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

void SplashLoginDialog::initResource()
{
    MQ_D(SplashLoginDialog);
    qApp->processEvents();

    addMessage(tr("start parse configure resource"));
    G_ParaSettings = new ParameterSettings::ParaSettings;
    QString localConfigName = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath) + QDir::separator() + QStringLiteral("参数配置.txt");
    if(!RSingleton<XMLParse>::instance()->parse(localConfigName,G_ParaSettings)){
        RMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Paramter settings read failed,system exit!"),
                             RMessageBox::Yes,RMessageBox::Yes);
        exit(-1);
    }

    UserBaseInfo baseInfo;
    baseInfo.accountId = G_ParaSettings->baseInfo.nodeId;
    baseInfo.nickName = G_ParaSettings->baseInfo.nodeId;
    G_User = new User(baseInfo);
    G_User->setTextOnline(true);
    G_User->setLogin(true);

    addMessage(tr("create system tray menu"));
    d->trayIcon = new SystemTrayIcon();
    d->trayIcon->setModel(SystemTrayIcon::System_Login);
    d->trayIcon->setVisible(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,true).toBool());
    d->trayIcon->setModel(SystemTrayIcon::System_Main);

    addMessage(tr("create main window"));
    if(!d->mainDialog){
        d->mainDialog = new MainDialog();
    }

    RSingleton<NotifyWindow>::instance()->bindTrayIcon(d->trayIcon);
    connect(RSingleton<NotifyWindow>::instance(),SIGNAL(showSystemNotifyInfo(NotifyInfo,int)),this,SLOT(viewSystemNotify(NotifyInfo,int)));
    connect(RSingleton<NotifyWindow>::instance(),SIGNAL(ignoreAllNotifyInfo()),d->trayIcon,SLOT(removeAll()));
    connect(d->trayIcon,SIGNAL(showNotifyInfo(QString)),RSingleton<NotifyWindow>::instance(),SLOT(viewNotify(QString)));

    d->mainDialog->setLogInState(STATUS_ONLINE);

    QDateTime n = QDateTime::currentDateTime();
    QDateTime now;
    do{
        now = QDateTime::currentDateTime();
    }
    while(n.secsTo(now) <= 2);

    hide();
    d->mainDialog->show();
}

void SplashLoginDialog::addMessage(const QString &text)
{
    showMessage(text,Qt::AlignBottom|Qt::AlignCenter,Qt::white);
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

void SplashLoginDialog::openChatDialog(QString accountId)
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
