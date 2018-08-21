#include "notifywindow.h"

#include <Windows.h>

#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMetaEnum>
#include <QPropertyAnimation>
#include <QStateMachine>
#include <QState>
#include <QSignalTransition>
#include <QMap>
#include <QStyle>

#include "head.h"
#include "constants.h"
#include "../protocol/datastruct.h"
#include "global.h"
#include "toolbar.h"
#include "toolbox/listbox.h"
#include "toolbox/toolitem.h"
#include "util/rsingleton.h"
#include "Util/imagemanager.h"
#include "Util/rutil.h"
#include "widget/rbutton.h"
#include "user/user.h"
#include "systemtrayicon.h"

#pragma comment(lib,"user32.lib")

#define NOTIFY_WINDOW_WIDTH 300
#define NOTIFY_WINDOW_HEIGHT 400

class NotifyWindowPrivate : public GlobalData<NotifyWindow>
{
    Q_DECLARE_PUBLIC(NotifyWindow)
public:
    NotifyWindowPrivate(NotifyWindow * q):q_ptr(q),
        propertName("NotifyType"),porpertyId("AccountId"),notifId("NotifyId")
    {
        //获取任务栏的尺寸
        ::GetWindowRect(::FindWindow(TEXT("Shell_TrayWNd"), NULL), &taskBarRect);
        toolBar = NULL;
        initWidget();
        initStateMachine();
    }

    void enableButt(RButton * butt,bool enable){
        butt->style()->unpolish(butt);
        butt->setEnabled(enable);
        butt->style()->polish(butt);
    }

private:
    void initWidget();
    void initStateMachine();

    NotifyWindow * q_ptr;
    RECT taskBarRect;

    ListBox * infoList;
    QMap<QString,int> ignoreList;
    RButton * ignoreButt;
    RButton * viewAllButt;
    ToolBar * toolBar;

    const QString propertName;
    const QString porpertyId;
    const QString notifId;

    QStateMachine * stateMachine;

    std::map<ToolItem*,NotifyInfo> systemNotifyInfos;

    SystemTrayIcon * systemTrayIcon;
};

void NotifyWindowPrivate::initWidget()
{
    QWidget * contentWidget = new QWidget();
    contentWidget->setObjectName("Widget_ContentWidget");

    QWidget * toolWidget = new QWidget(contentWidget);
    QHBoxLayout * toolLayout = new QHBoxLayout();
    toolLayout->setContentsMargins(0,0,0,0);
    toolLayout->setSpacing(0);

    ignoreButt = new RButton(toolWidget);
    ignoreButt->setText(QObject::tr("Ignore"));

    viewAllButt = new RButton(toolWidget);
    viewAllButt->setText(QObject::tr("View all"));

    enableButt(ignoreButt,false);
    enableButt(viewAllButt,false);

    QObject::connect(ignoreButt,SIGNAL(pressed()),q_ptr,SLOT(ignoreAll()));
    QObject::connect(viewAllButt,SIGNAL(pressed()),q_ptr,SLOT(viewAll()));

    toolLayout->addWidget(ignoreButt);
    toolLayout->addStretch(1);
    toolLayout->addWidget(viewAllButt);
    toolWidget->setLayout(toolLayout);

    infoList = new ListBox(contentWidget);
    infoList->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->addWidget(infoList);
    contentLayout->addWidget(toolWidget);

    contentWidget->setLayout(contentLayout);
    q_ptr->setContentWidget(contentWidget);
}

void NotifyWindowPrivate::initStateMachine()
{
    stateMachine = new QStateMachine();

    QSize defaultWindowSize = RUtil::screenSize();

    QState * state1 = new QState(stateMachine);
    state1->assignProperty(q_ptr,"geometry",QRect(defaultWindowSize.width() - NOTIFY_WINDOW_WIDTH,defaultWindowSize.height(),
                                                  NOTIFY_WINDOW_WIDTH,NOTIFY_WINDOW_HEIGHT));
    stateMachine->setInitialState(state1);

    QState * state2 = new QState(stateMachine);
    state2->assignProperty(q_ptr,"geometry",QRect(defaultWindowSize.width() - NOTIFY_WINDOW_WIDTH,taskBarRect.top - NOTIFY_WINDOW_HEIGHT,
                                                  NOTIFY_WINDOW_WIDTH,NOTIFY_WINDOW_HEIGHT));

    QSignalTransition * transition1 = state1->addTransition(q_ptr,SIGNAL(showWindow()),state2);
    transition1->addAnimation(new QPropertyAnimation(q_ptr,"geometry"));

    QSignalTransition * transition2 = state2->addTransition(q_ptr,SIGNAL(hideWindow()),state1);
    transition2->addAnimation(new QPropertyAnimation(q_ptr,"geometry"));

    stateMachine->start();
}

NotifyWindow::NotifyWindow(QWidget *parent):
    Widget(parent),
    d_ptr(new NotifyWindowPrivate(this))
{
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));
    setToolBarMoveable(true);
    d_ptr->toolBar = enableToolBar(true);
    enableDefaultSignalConection(false);
    if(d_ptr->toolBar)
    {
        d_ptr->toolBar->setToolFlags((ToolBar::ToolType)(ToolBar::TOOL_DIALOG & ~ToolBar::TOOL_CLOSE));
        d_ptr->toolBar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM,ImageManager::ICON_16));
        d_ptr->toolBar->setWindowTitle(G_User->BaseInfo().nickName);
        connect(this,SIGNAL(minimumWindow()),this,SIGNAL(hideWindow()));
    }
    setMaximumWidth(NOTIFY_WINDOW_WIDTH);
    setMaximumHeight(NOTIFY_WINDOW_HEIGHT);

    RSingleton<Subject>::instance()->attach(this);
}

NotifyWindow::~NotifyWindow()
{
    RSingleton<Subject>::instance()->detach(this);
}

void NotifyWindow::bindTrayIcon(SystemTrayIcon *trayIcon)
{
    MQ_D(NotifyWindow);
    d->systemTrayIcon = trayIcon;
}

/*!
 * @brief 向通知栏添加一个通知消息
 * @details 若该消息所属的accountId列表不存在，创建该消息提示；若accountId存在，根据消息的类型来进行不同处理：
 *          若属于系统消息(20180426修复添加显示系统信息时未考虑群请求特殊情况):
 *              好友请求：已存在该accountId的消息，在未处理时，则进行忽略(多次发送一个好友请求意义不大)；
 *              群请求：已存在对应群id请求消息时，在未处理时进行忽略；若一个accountdId请求不同的的群，则需分别进行提示：
 *          若属于联系人消息，已存在该accountId的消息，未处理时，在通知消息数量上累加。
 * @param[in] info 通知消息
 * @return 若为新插入，返回新插入的ID；否则返回已经存在的info的id
 */
QString NotifyWindow::addNotifyInfo(NotifyInfo &info)
{
    MQ_D(NotifyWindow);

    QString existInfoIdeitity;
    bool existedUser = false;

    QList<ToolItem *> items = d->infoList->items();
    QList<ToolItem *>::iterator iter = items.begin();
    while(iter != items.end())
    {
        ToolItem * curItem = *iter;
        NotifyInfo tmpInfo = d->systemNotifyInfos.at(curItem);
        NotifyType itemType = (NotifyType)curItem->property(d->propertName.toLocal8Bit().data()).toInt();
        QString accountId = curItem->property(d->porpertyId.toLocal8Bit().data()).toString();

        if(accountId == info.accountId){
            if(info.stype == OperatePerson){
                if(itemType == NotifyUser || itemType == NotifyGroup){
                    curItem->addNotifyInfo();
                    existInfoIdeitity = d->systemNotifyInfos.at(curItem).identityId;
                }
                existedUser = true;
            }else if(info.stype == OperateGroup){
                if(info.chatId == tmpInfo.chatId){
                    existedUser = true;
                }
            }
            break;
        }
        iter++;
    }

    if(!existedUser)
    {
        ToolItem * item = new ToolItem(NULL);
        connect(item,SIGNAL(itemDoubleClick(ToolItem*)),this,SLOT(viewNotify(ToolItem *)));
        item->setProperty(d->propertName.toLocal8Bit().data(),info.type);
        item->setProperty(d->porpertyId.toLocal8Bit().data(),info.accountId);

        if(info.type == NotifySystem){
            item->setName(QObject::tr("System info"));
            item->setIcon(info.iconId);
        }else if(info.type == NotifyUser || info.type == NotifyGroup){
            item->setName(info.accountId);
            item->setNickName(info.nickName);
            item->setDescInfo(info.content);
            item->setIcon(G_User->getIcon(info.isSystemIcon,info.iconId));
        }
        item->addNotifyInfo();
        d->infoList->addItem(item);
        d->systemNotifyInfos.insert(std::make_pair(item,info));
        existInfoIdeitity = info.identityId;

        if(!d->ignoreButt->isEnabled()){
            d->enableButt(d->ignoreButt,true);
            d->enableButt(d->viewAllButt,true);
        }
    }

    if(existInfoIdeitity == info.identityId){
        if(info.type == NotifySystem){
            d->systemTrayIcon->notify(SystemTrayIcon::SystemNotify,info.identityId);
        }else{
            d->systemTrayIcon->notify(SystemTrayIcon::UserNotify,info.identityId,G_User->getIcon(info.isSystemIcon,info.iconId));
        }
    }else{
        if(info.type == NotifySystem){
            d->systemTrayIcon->frontNotify(SystemTrayIcon::SystemNotify,existInfoIdeitity);
        }else{
            d->systemTrayIcon->frontNotify(SystemTrayIcon::UserNotify,existInfoIdeitity);
        }
    }

    return existInfoIdeitity;
}

void NotifyWindow::showMe()
{
    show();
    emit showWindow();
}

void NotifyWindow::hideMe()
{
    emit hideWindow();
}

void NotifyWindow::onMessage(MessageType type)
{
    MQ_D(NotifyWindow);
    if(type == MESS_BASEINFO_UPDATE){
        d->toolBar->setWindowTitle(G_User->BaseInfo().nickName);
    }
}

/*!
 * @brief  检测目标id是否存在未读取的信息，包括被忽略的信息
 * @param accountId 目标id
 * @return 与目标id存在的待推送消息条数
 */
int NotifyWindow::checkNotifyExist(const QString accountId)
{
    MQ_D(NotifyWindow);

    int t_count = 0;
    t_count += checkNotifyIgnoreItems(accountId);
    t_count += checkNotifyExistItems(accountId);
    return t_count;
}

/*!
 * @brief  检测目标id是否存在推送消息
 * @param accountId 目标id
 * @return 与目标id存在的待推送消息条数
 */
int NotifyWindow::checkNotifyExistItems(const QString accountId)
{
    MQ_D(NotifyWindow);

    int t_count = 0;
    foreach (ToolItem * curItem, d->infoList->items())
    {
        if(curItem->getName() == accountId)
        {
            t_count = curItem->getNotifyCount();
            auto iter = d->systemNotifyInfos.find(curItem);
            if(iter != d->systemNotifyInfos.end())
            {
                d->systemNotifyInfos.erase(iter);
            }
            d->infoList->removeItem(curItem);
            if(d->infoList->count() <= 0){
                ignoreAll();
            }
            break;
        }
    }
    return t_count;
}

/*!
 * @brief  检测目标id是否存在被忽略的推送消息
 * @param accountId 目标id
 * @return 与目标id存在的被忽略的推送消息条数
 */
int NotifyWindow::checkNotifyIgnoreItems(const QString accountId)
{
    MQ_D(NotifyWindow);

    int t_count = d->ignoreList.value(accountId);
    d->ignoreList.remove(accountId);
    return t_count;
}

void NotifyWindow::viewAll()
{
    MQ_D(NotifyWindow);
    QList<ToolItem *> items = d->infoList->items();
    QList<ToolItem *>::iterator iter = items.end();
    while(iter != items.begin()){
        iter--;
        viewNotify(*iter);
    }
}

void NotifyWindow::ignoreAll()
{
    MQ_D(NotifyWindow);

    saveIgnoreNotify();

    hideMe();
    d->enableButt(d->ignoreButt,false);
    d->enableButt(d->viewAllButt,false);

    d->systemNotifyInfos.clear();
    d->infoList->clear();
    emit ignoreAllNotifyInfo();
}

/*!
 * @brief 保存被忽略的推送信息
 */
void NotifyWindow::saveIgnoreNotify()
{
    MQ_D(NotifyWindow);

    int t_count = 0;
    foreach (ToolItem * curItem, d->infoList->items())
    {
        t_count = curItem->getNotifyCount();
        t_count += d->ignoreList.value(curItem->getName());
        d->ignoreList.insert(curItem->getName(),t_count);
    }
}

/*!
 * @brief 查看通知详细内容
 * @details 响应双击查看内容，查看后对应的消息ID从列表中移除。
 * @param[in] item
 * @return 无
 */
void NotifyWindow::viewNotify(ToolItem *item)
{
    MQ_D(NotifyWindow);

    if(item){
        emit showSystemNotifyInfo(d->systemNotifyInfos.at(item),item->getNotifyCount());

        auto iter = d->systemNotifyInfos.find(item);
        if(iter != d->systemNotifyInfos.end()){
            d->systemNotifyInfos.erase(iter);
        }

        if(d->infoList->count() <= 0){
            ignoreAll();
        }
    }
}

/*!
 * @brief 响应托盘双击查看当前闪烁的通知消息
 * @param[in] notifyId 待查看通知消息ID
 */
void NotifyWindow::viewNotify(QString notifyId)
{
    MQ_D(NotifyWindow);

    auto iter = std::find_if(d->systemNotifyInfos.begin(),d->systemNotifyInfos.end(),[&notifyId](std::pair<ToolItem*,NotifyInfo> item){
        return item.second.identityId == notifyId;
    });

    if(iter != d->systemNotifyInfos.end()){
        viewNotify((*iter).first);
    }
}
