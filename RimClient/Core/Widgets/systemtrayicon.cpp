#include "systemtrayicon.h"

#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QTimer>
#include <QMap>

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
    int notifyCount;                        //用于切换闪烁托盘图标信息
    QString blinkingNotifyId;               //闪烁时通知的id，与NotifyWinwos中通知id保持一致

    QList<SystemTrayIcon::NotifyDesc> notifyList;

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
    if(systemIcon){
        systemIcon = NULL;
    }

    if(d_ptr){
        if(d->blinkingTimer){
            if(d->blinkingTimer->isActive()){
                d->blinkingTimer->stop();
            }
            delete d->blinkingTimer;
        }
    }
}

/*!
 * @brief 设置工作模式
 * @details 托盘在创建时被登陆页面以及主页面共享使用，在两种状态下，托盘所具备的功能是不一致的。
 * @param[in] model 工作模式
 * @return 无
 */
void SystemTrayIcon::setModel(SystemTrayIcon::SystemTrayModel model)
{
    MQ_D(SystemTrayIcon);
    d->setModel(model);

    if(model == System_Main){
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
    }else if(model == System_Login){
        setContextMenu(d->trayLoginMenu);
    }
}

/*!
 * @brief 添加通知
 * @details 将新通知加入到通知列表，并将通知进行统一保存。
 * @param[in] model 通知类型
 * @param[in] id 通知消息id，此id和NotifyWindow中id保持一致，用于标识消息身份；若id为默认值，则此消息为系统消息或无通知
 * @param[in] imagePath 图片路径
 * @return 无
 */
void SystemTrayIcon::notify(SystemTrayIcon::NotifyModel model, QString id,QString imagePath)
{
    MQ_D(SystemTrayIcon);
    d->notifyModel = model;
    d->notifyCount = 0;

    if(model != NoneNotify)
    {
        NotifyDesc notify;
        notify.model = model;
        notify.id = id;
        if(model == SystemNotify){
            notify.imagePath = RSingleton<ImageManager>::instance()->getIcon(ImageManager::ICON_SYSTEMNOTIFY,ImageManager::ICON_24);
            imagePath = notify.imagePath;
        }else{
            notify.imagePath = imagePath;
        }
        d->blinkingNotifyId = id;
        d->notifyList.append(notify);
    }

    switch(model)
    {
        case NoneNotify:
            {
                if(d->blinkingTimer && d->blinkingTimer->isActive()){
                    d->blinkingTimer->stop();
                }
                d->blinkingIcon = QIcon(RSingleton<ImageManager>::instance()->getWindowIcon());
                setIcon(d->blinkingIcon);
                break;
            }
        case SystemNotify:
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

/*!
 * @brief 置顶闪烁显示消息
 * @details 当存在多个不同用户消息时，再次接收到某个联系人消息时，则将其置顶闪烁显示
 * @param[in] model 消息类型
 * @param[in] id 消息ID
 * @return 无
 */
void SystemTrayIcon::frontNotify(NotifyModel model, QString id)
{
    MQ_D(SystemTrayIcon);
    d->blinkingNotifyId = id;
    if(model == UserNotify)
    {
        QList<SystemTrayIcon::NotifyDesc>::iterator iter = d->notifyList.begin();
        while(iter != d->notifyList.end())
        {
            if((*iter).id == id)
            {
                d->blinkingIcon = QIcon((*iter).imagePath);
                startBliking();
                break;
            }
            iter++;
        }
    }
    else if(model == SystemNotify)
    {
        d->blinkingIcon = QIcon(RSingleton<ImageManager>::instance()->getIcon(ImageManager::ICON_SYSTEMNOTIFY,ImageManager::ICON_24));
        startBliking();
    }
}

/*!
 * @brief 移除通知消息
 * @details 根据传入的ID，从通知列表中移除。若还有剩余消息，则将最后一个消息头像作为通知闪烁头像；若消息列表为空，则将托盘图标设置为系统默认图像，并停止闪烁。
 * @see stopBliking
 * @param[in] id 待移除的通知ID
 * @return 无
 */
void SystemTrayIcon::removeNotify(QString id)
{
    MQ_D(SystemTrayIcon);

    if(d->notifyList.size() > 0)
    {
        bool cleared = false;
        QList<SystemTrayIcon::NotifyDesc>::iterator iter = d->notifyList.begin();
        while(iter != d->notifyList.end())
        {
            if((*iter).id == id)
            {
                d->notifyList.erase(iter);
                cleared = true;
                break;
            }
            iter++;
        }

        if(cleared)
        {
            if(d->notifyList.size() > 0)
            {
                d->blinkingIcon = QIcon(d->notifyList.last().imagePath);
                d->blinkingNotifyId = d->notifyList.last().id;
            }
            else
            {
                stopBliking();

                d->blinkingIcon = QIcon(RSingleton<ImageManager>::instance()->getWindowIcon());
                d->blinkingNotifyId = QString();
                setIcon(d->blinkingIcon);
            }
        }
    }
}

void SystemTrayIcon::removeAll()
{
    MQ_D(SystemTrayIcon);

    if(d->notifyList.size() > 0)
    {
        QList<SystemTrayIcon::NotifyDesc>::iterator iter = d->notifyList.begin();
        while(iter != d->notifyList.end())
        {
            iter = d->notifyList.erase(iter);
        }

        stopBliking();
        d->blinkingIcon = QIcon(RSingleton<ImageManager>::instance()->getWindowIcon());
        setIcon(d->blinkingIcon);
    }
}

/*!
 * @brief 开启闪烁
 * @details 控制托盘状态闪烁，默认闪烁周期为400ms。
 * @param[in] interval 闪烁周期
 * @return 无
 */
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

void SystemTrayIcon::stopBliking()
{
    MQ_D(SystemTrayIcon);

    if(d->blinkingTimer && d->blinkingTimer->isActive())
    {
        d->blinkingTimer->stop();
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

/*!
 * @brief 处理对托盘系统的鼠标操作
 * @param[in] reason 触发托盘系统执行的不同条件
 */
void SystemTrayIcon::respIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    MQ_D(SystemTrayIcon);

    switch(reason)
    {
        case QSystemTrayIcon::DoubleClick:
              {
                   if(d->blinkingNotifyId.size() > 0){
                       emit showNotifyInfo(d->blinkingNotifyId);
                       return;
                   }
                   if(d->model() == System_Main){
                        emit showMainPanel();
                   }
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
