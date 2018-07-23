#include "chatpersonwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QPropertyAnimation>
#include <QNetworkProxyFactory>

#include "head.h"
#include "global.h"
#include "toolbar.h"
#include "document.h"
#include "user/user.h"
#include "constants.h"
#include "util/rsingleton.h"
#include "Util/rutil.h"
#include "notifywindow.h"
#include "widget/rlabel.h"
#include "widget/rbutton.h"
#include "Util/imagemanager.h"
#include "widget/rtoolbutton.h"
#include "thread/chatmsgprocess.h"
#include "abstractchatmainwidget.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define CHAT_TOOL_HEIGHT 30
#define CHAT_USER_ICON_SIZE 30
#define CHAT_SHAKE_DURATION 200             //窗口抖动动画持续时间
#define CHAT_SHAKE_LOOPCOUNT 2              //窗口抖动动画循环次数
#define CHAT_SHAKE_RANGE 6                  //窗口抖动幅度

class ChatPersonWidgetPrivate : public GlobalData<ChatPersonWidget>
{
    Q_DECLARE_PUBLIC(ChatPersonWidget)

protected:
    ChatPersonWidgetPrivate(ChatPersonWidget *q):
        q_ptr(q)
    {
        initWidget();
        windowId = RUtil::UUID();
        isLoadFinished = false;
    }

    void initWidget();

    ChatPersonWidget *q_ptr;

    QWidget * contentWidget;
    QWidget * userInfoWidget;               //用户头像等信息页面
    RIconLabel * iconLabel;                 //聊天对象头像
    QLabel * nameLabel;                     //聊天对象名称
    ToolBar * windowToolBar;                //窗口控制工具栏
    ToolBar * toolBar;                      //工具栏
    AbstractChatMainWidget *mainWidget;     //聊天内置窗口
    Document *m_bridge;                     //内置html与聊天窗口通信跳板

    QString windowId;
    SimpleUserInfo m_userInfo;              //当前聊天对象基本信息
    bool isMaxSize;
    bool isLoadFinished;                    //标记当前内置的html是否加载完成
    ChatInfoUnitList tempData;              //临时缓存信息
};

void ChatPersonWidgetPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("ChatC2CWidget");
    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    /**********用户信息***************/
    userInfoWidget = new QWidget(contentWidget);
    userInfoWidget->setObjectName("ChatC2CWidget_TopItem");
    userInfoWidget->setFixedHeight(50);

    QHBoxLayout * userLayout = new QHBoxLayout;
    userLayout->setContentsMargins(6,0,6,0);
    userLayout->setSpacing(5);

    iconLabel = new RIconLabel(userInfoWidget);
    iconLabel->setTransparency(true);
    iconLabel->setFixedSize(CHAT_USER_ICON_SIZE,CHAT_USER_ICON_SIZE);

    nameLabel = new QLabel(userInfoWidget);
    nameLabel->setObjectName("Chat_User_NameLabel");
    nameLabel->setFixedHeight(CHAT_USER_ICON_SIZE);

    /**********窗口控制区_顶部***************/
    windowToolBar = new ToolBar(contentWidget);
    windowToolBar->setContentsMargins(5,0,0,0);

    windowToolBar->setToolFlags(ToolBar::TOOL_ACTION);
    QObject::connect(windowToolBar,SIGNAL(minimumWindow()),q_ptr,SLOT(showMinimized()));
    QObject::connect(windowToolBar,SIGNAL(closeWindow()),q_ptr,SLOT(hide()));
    QObject::connect(windowToolBar,SIGNAL(maxWindow(bool)),q_ptr,SLOT(showMaximizedWindow(bool)));

    userLayout->addWidget(iconLabel);
    userLayout->addWidget(nameLabel);
    userLayout->addStretch(1);
    userLayout->addWidget(windowToolBar);
    userInfoWidget->setLayout(userLayout);

    /**********工具栏***************/
    toolBar = new ToolBar(contentWidget);
    toolBar->setObjectName("ChatC2CWidget_TopToolbar");
    toolBar->setFixedHeight(CHAT_TOOL_HEIGHT);

    RToolButton * callButton = new RToolButton(toolBar);
    callButton->setObjectName(Constant::Tool_Chat_Call);
    callButton->setFixedSize(28,24);

    toolBar->appendToolButton(callButton);
    toolBar->addStretch(1);

    mainWidget = new AbstractChatMainWidget(contentWidget);
    mainWidget->setChatType(AbstractChatMainWidget::C2C);
    QObject::connect(mainWidget,SIGNAL(shakeWindow()),q_ptr,SLOT(shakeWindow()));
    QObject::connect(mainWidget,SIGNAL(closeWindow()),q_ptr,SLOT(hide()));
    QObject::connect(mainWidget,SIGNAL(initFinished()),q_ptr,SLOT(autoQueryRecord()));
    QObject::connect(q_ptr,SIGNAL(sendQueryRecord(const ChatInfoUnit &)),mainWidget,SLOT(showQueryRecord(const ChatInfoUnit &)));
    QObject::connect(q_ptr,SIGNAL(sendRecvedMsg(const TextRequest &)),mainWidget,SLOT(recvTextChatMsg(const TextRequest &)));
    QObject::connect(q_ptr,SIGNAL(sendRecvedAudio(QString)),mainWidget,SLOT(recvVoiceChatMsg(QString)));
    QObject::connect(q_ptr,SIGNAL(sendMsgStatus(ushort)),mainWidget,SLOT(updateMsgStatus(ushort)));
    QObject::connect(q_ptr,SIGNAL(sendMoreQueryRecord(const ChatInfoUnit &,bool)),mainWidget,SLOT(showMoreQueryRecord(const ChatInfoUnit &,bool)));

    contentLayout->addWidget(userInfoWidget);
    contentLayout->addWidget(toolBar);
    contentLayout->addWidget(mainWidget);
    contentWidget->setLayout(contentLayout);
    q_ptr->setContentWidget(contentWidget);

    m_bridge = new Document;
    m_bridge->setUi(mainWidget);
    QWebChannel *channel = new QWebChannel(mainWidget);
    channel->registerObject(QStringLiteral("bridge"),m_bridge);
    mainWidget->setChatChannel(channel);
}

ChatPersonWidget::ChatPersonWidget(QWidget *parent):
    d_ptr(new ChatPersonWidgetPrivate(this)),
    Widget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() &(~Qt::Tool));
    QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::NoProxy));
    QNetworkProxyFactory::setUseSystemConfiguration(false);
    d_ptr->userInfoWidget->installEventFilter(this);
    d_ptr->windowToolBar->installEventFilter(this);

    RSingleton<Subject>::instance()->attach(this);
}

ChatPersonWidget::~ChatPersonWidget()
{

}

void ChatPersonWidget::onMessage(MessageType type)
{
    switch(type)
    {
        case MESS_SHORTCUT:
           {
                break;
           }
        default:
                break;
    }
}

/*!
 * @brief 初始化聊天记录
 */
void ChatPersonWidget::initChatRecord()
{
    ChatMsgProcess *chatProcess = RSingleton<ChatMsgProcess>::instance();
    connect(chatProcess,SIGNAL(C2CResultReady(ChatInfoUnitList)),
            this,SLOT(queryRecordReady(ChatInfoUnitList)));
    connect(chatProcess,SIGNAL(C2CMsgStatusChanged(ushort,ushort)),
            this,SLOT(updateMsgStatus(ushort,ushort)));
    connect(chatProcess,SIGNAL(C2CMoreResultReady(ChatInfoUnitList)),
            this,SLOT(queryMoreRecordReady(ChatInfoUnitList)));
    connect(chatProcess,SIGNAL(finished()),
            chatProcess,SLOT(deleteLater()));
}

/*!
 * @brief 显示联系人基本信息
 * @param info 联系人基本信息
 */
void ChatPersonWidget::setUserInfo(const SimpleUserInfo &info)
{
    MQ_D(ChatPersonWidget);
    d->m_userInfo = info;
    d->mainWidget->setUserInfo(info);
    d->nameLabel->setText(info.nickName);
    d->iconLabel->setPixmap(G_User->getIcon(info.isSystemIcon,info.iconId));
    setWindowIcon(RSingleton<ImageManager>::instance()->getCircularIcons(G_User->getIcon(info.isSystemIcon,info.iconId)));
    setWindowTitle(info.nickName);
}


#ifdef __LOCAL_CONTACT__
/*!
 * @brief 设置节点外发信息配置
 * @param[in] config 节点外发配置信息
 * @note 该配置信息描述了与当前节点通信的网络描述信息，包括使用的通信方式，报文格式等。
 */
void ChatPersonWidget::setOuterNetConfig(const ParameterSettings::OuterNetConfig &config)
{
    MQ_D(ChatPersonWidget);
    d->mainWidget->setOuterNetConfig(config);
}
#endif

/*!
 * @brief 转发收到的聊天信息
 * @param msg 收到的聊天信息
 */
void ChatPersonWidget::recvChatMsg(const TextRequest &msg)
{
    emit sendRecvedMsg(msg);
}

/*!
 * @brief 转发收到的语音消息
 * @param fileName 语音文件名称
 */
void ChatPersonWidget::recvChatAudio(QString fileName)
{
    emit sendRecvedAudio(fileName);
}

/*!
 * @brief 查询显示最近的几条信息
 * @param count 信息数目
 */
void ChatPersonWidget::showRecentlyChatMsg(uint count)
{
     MQ_D(ChatPersonWidget);

    uint start = 0;
    RSingleton<ChatMsgProcess>::instance()->appendC2CQueryTask(d->m_userInfo.accountId,start,count);

    RSingleton<NotifyWindow>::instance()->checkNotifyExist(d->m_userInfo.accountId);
}

bool ChatPersonWidget::eventFilter(QObject *watched, QEvent *event)
{
    MQ_D(ChatPersonWidget);

    if(watched == d->userInfoWidget)
    {
        if(event->type() == QEvent::MouseButtonDblClick)
        {
            switchWindowSize();
            return true;
        }
    }
    else if(watched == d->windowToolBar)
    {
        if(event->type() == QEvent::MouseButtonDblClick)
        {
            switchWindowSize();
            return true;
        }
    }

    return Widget::eventFilter(watched,event);
}

/*!
 * @brief 抖动窗口
 */
void ChatPersonWidget::shakeWindow()
{
    QPoint pos = this->pos();
    QPropertyAnimation *pAnimation = new QPropertyAnimation(this, "pos");
    pAnimation->setDuration(CHAT_SHAKE_DURATION);
    pAnimation->setLoopCount(CHAT_SHAKE_LOOPCOUNT);
    pAnimation->setStartValue(pos);
    pAnimation->setKeyValueAt(0.1,pos + QPoint(-CHAT_SHAKE_RANGE,-CHAT_SHAKE_RANGE));
    pAnimation->setKeyValueAt(0.2,pos + QPoint(-CHAT_SHAKE_RANGE,0));
    pAnimation->setKeyValueAt(0.3,pos + QPoint(-CHAT_SHAKE_RANGE,CHAT_SHAKE_RANGE));
    pAnimation->setKeyValueAt(0.4,pos + QPoint(0,CHAT_SHAKE_RANGE));
    pAnimation->setKeyValueAt(0.5,pos + QPoint(CHAT_SHAKE_RANGE,CHAT_SHAKE_RANGE));
    pAnimation->setKeyValueAt(0.6,pos + QPoint(CHAT_SHAKE_RANGE,0));
    pAnimation->setKeyValueAt(0.7,pos + QPoint(CHAT_SHAKE_RANGE,-CHAT_SHAKE_RANGE));
    pAnimation->setKeyValueAt(0.8,pos + QPoint(0,-CHAT_SHAKE_RANGE));
    pAnimation->setKeyValueAt(0.9,pos + QPoint(-CHAT_SHAKE_RANGE,-CHAT_SHAKE_RANGE));
    pAnimation->setEndValue(pos);
    pAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

/*!
 * @brief 响应查询聊天记录结果
 */
void ChatPersonWidget::queryRecordReady(ChatInfoUnitList historyMsgs)
{
    foreach(ChatInfoUnit unit,historyMsgs)
    {
        emit sendQueryRecord(unit);
    }

}

void ChatPersonWidget::queryMoreRecordReady(ChatInfoUnitList moreMsgs)
{
    MQ_D(ChatPersonWidget);

    for(int index=0;index<moreMsgs.count();index++)
    {
        ChatInfoUnit unit = moreMsgs.at(index);
        bool hasNext = (index+1)<moreMsgs.count();
        if(hasNext)
        {
            d->mainWidget->setNextDateTime(RUtil::addMSecsToEpoch(unit.dtime));
        }
        emit sendMoreQueryRecord(unit,hasNext);
    }
}

/*!
 * @brief html加载完成后加载待显示消息（推送消息/历史记录消息）
 */
void ChatPersonWidget::autoQueryRecord()
{
    MQ_D(ChatPersonWidget);

    d->isLoadFinished = true;

    int t_notifyCount = RSingleton<NotifyWindow>::instance()->checkNotifyExist(d->m_userInfo.accountId);

    if(t_notifyCount)
    {
        if(t_notifyCount > 20)
        {
            showRecentlyChatMsg(20);
        }
        else
        {
            showRecentlyChatMsg(t_notifyCount);
        }
    }
    else
    {
        showRecentlyChatMsg(3);
    }
}

void ChatPersonWidget::showMaximizedWindow(bool flag)
{
    Q_UNUSED(flag);
    showMaximized();
}

/*!
 * @brief 更新界面文字已读/未读状态
 * @param serialNo 文字流水号
 */
void ChatPersonWidget::updateMsgStatus(ushort id,ushort serialNo)
{
    emit sendMsgStatus(serialNo);
}

/*!
 * @brief 切换窗口尺寸
 */
void ChatPersonWidget::switchWindowSize()
{
    MQ_D(ChatPersonWidget);

#ifdef Q_OS_WIN
    WINDOWPLACEMENT wp;
    wp.length = sizeof( WINDOWPLACEMENT );

    HWND hd = (HWND)Widget::winId();
    GetWindowPlacement( hd, &wp );
    if ( wp.showCmd == SW_MAXIMIZE )
    {
        ShowWindow( hd, SW_RESTORE );
    }
    else
    {
        showMaximized();
    }
#endif
    d->isMaxSize = !d->isMaxSize;

    setShadowWindow(!isMaximized());
}
