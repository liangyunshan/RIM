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

#include "head.h"
#include "constants.h"
#include "datastruct.h"
#include "global.h"
#include "toolbar.h"
#include "toolbox/listbox.h"
#include "toolbox/toolitem.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "Util/rutil.h"
#include "widget/rbutton.h"

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

private:
    void initWidget();
    void initStateMachine();

    NotifyWindow * q_ptr;
    RECT taskBarRect;

    ListBox * infoList;
    RButton * ignoreButt;
    RButton * viewAllButt;
    ToolBar * toolBar;

    const QString propertName;
    const QString porpertyId;
    const QString notifId;

    QStateMachine * stateMachine;

    QMap<ToolItem*,NotifyInfo> systemNotifyInfos;
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

    d_ptr->toolBar = enableToolBar(true);
    enableDefaultSignalConection(false);
    if(d_ptr->toolBar)
    {
        d_ptr->toolBar->setToolFlags((ToolBar::ToolType)(ToolBar::TOOL_DIALOG & ~ToolBar::TOOL_CLOSE));
        d_ptr->toolBar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM,ImageManager::ICON_16));
        d_ptr->toolBar->setWindowTitle(G_UserBaseInfo.nickName);
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

void NotifyWindow::addNotifyInfo(NotifyInfo info)
{
    MQ_D(NotifyWindow);

    bool existedUser = false;

    QList<ToolItem *> items = d->infoList->items();
    QList<ToolItem *>::iterator iter = items.begin();
    while(iter != items.end())
    {
        ToolItem * curItem = *iter;
        NotifyType itemType = (NotifyType)curItem->property(d->propertName.toLocal8Bit().data()).toInt();
        QString accountId = curItem->property(d->porpertyId.toLocal8Bit().data()).toString();

        if(accountId == info.accountId)
        {
            if(itemType == NotifyUser || itemType == NotifyGroup)
            {
                curItem->addNotifyInfo();
            }
            existedUser = true;
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

        if(info.type == NotifySystem)
        {
            item->setName(QObject::tr("System info"));
            d->systemNotifyInfos.insert(item,info);
        }
        else if(info.type == NotifyUser || info.type == NotifyGroup)
        {
            item->setName(info.accountId);
            item->setNickName(info.nickName);
            item->setDescInfo(info.content);
        }
        item->setIcon(info.pixmap);
        item->addNotifyInfo();
        d->infoList->addItem(item);
    }
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
    if(type == MESS_BASEINFO_UPDATE)
    {
        d->toolBar->setWindowTitle(G_UserBaseInfo.nickName);
    }
}

void NotifyWindow::resizeEvent(QResizeEvent *)
{

}

void NotifyWindow::viewAll()
{
    hideMe();
}

void NotifyWindow::ignoreAll()
{
    MQ_D(NotifyWindow);

    hideMe();
    d->infoList->clear();
}

void NotifyWindow::viewNotify(ToolItem *item)
{
    MQ_D(NotifyWindow);

    if(item)
    {
        emit showSystemNotifyInfo(d->systemNotifyInfos.value(item));
        d->systemNotifyInfos.remove(item);
        d->infoList->removeItem(item);

        if(d->infoList->count() <= 0)
        {
            ignoreAll();
        }
    }
}
