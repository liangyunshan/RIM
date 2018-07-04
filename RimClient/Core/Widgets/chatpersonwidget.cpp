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
#include "rsingleton.h"
#include "Util/rutil.h"
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
    userInfoWidget->setFixedHeight(50);

    QHBoxLayout * userLayout = new QHBoxLayout;
    userLayout->setContentsMargins(6,0,0,0);
    userLayout->setSpacing(5);

    iconLabel = new RIconLabel(userInfoWidget);
    iconLabel->setTransparency(true);
    iconLabel->setFixedSize(CHAT_USER_ICON_SIZE,CHAT_USER_ICON_SIZE);

    nameLabel = new QLabel(userInfoWidget);
    nameLabel->setObjectName("Chat_User_NameLabel");
    nameLabel->setFixedHeight(CHAT_USER_ICON_SIZE);

    userLayout->addWidget(iconLabel);
    userLayout->addWidget(nameLabel);
    userLayout->addStretch(1);
    userInfoWidget->setLayout(userLayout);

    /**********工具栏***************/
    toolBar = new ToolBar(contentWidget);
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
    QObject::connect(q_ptr,SIGNAL(sendQueryRecord(const ChatInfoUnit &)),mainWidget,SLOT(showQueryRecord(const ChatInfoUnit &)));
    QObject::connect(q_ptr,SIGNAL(sendRecvedMsg(const TextRequest &)),mainWidget,SLOT(recvTextChatMsg(const TextRequest &)));
    QObject::connect(q_ptr,SIGNAL(sendRecvedAudio(QString)),mainWidget,SLOT(recvVoiceChatMsg(QString)));

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

    /**********窗口控制区***************/
    windowToolBar = new ToolBar(contentWidget);
    windowToolBar->setContentsMargins(5,0,0,0);

    windowToolBar->setToolFlags(ToolBar::TOOL_ACTION);
    QObject::connect(windowToolBar,SIGNAL(minimumWindow()),q_ptr,SLOT(showMinimized()));
    QObject::connect(windowToolBar,SIGNAL(closeWindow()),q_ptr,SLOT(hide()));
}

ChatPersonWidget::ChatPersonWidget(QWidget *parent):
    d_ptr(new ChatPersonWidgetPrivate(this)),
    Widget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() &(~Qt::Tool));
    QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::NoProxy)); //FIXME LYS-20180607
    QNetworkProxyFactory::setUseSystemConfiguration(false); //FIXME LYS-20180607
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
 * @brief ChatPersonWidget::initChatRecord 初始化聊天记录
 */
void ChatPersonWidget::initChatRecord()
{
    ChatMsgProcess *chatProcess = RSingleton<ChatMsgProcess>::instance();
    connect(chatProcess,SIGNAL(C2CResultReady(ChatInfoUnitList &)),
            this,SLOT(queryRecordReady(ChatInfoUnitList &)));
    connect(chatProcess,SIGNAL(finished()),
            chatProcess,SLOT(deleteLater()));
//    showRecentlyChatMsg(3);
}

/*!
 * @brief ChatPersonWidget::setUserInfo 显示联系人基本信息
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
 * @brief ChatPersonWidget::recvChatMsg 转发收到的聊天信息
 * @param msg 收到的聊天信息
 */
void ChatPersonWidget::recvChatMsg(const TextRequest &msg)
{
    emit sendRecvedMsg(msg);
}

/*!
 * @brief ChatPersonWidget::recvChatAudio 转发收到的语音消息
 * @param fileName 语音文件名称
 */
void ChatPersonWidget::recvChatAudio(QString fileName)
{
    emit sendRecvedAudio(fileName);
}

/*!
 * @brief ChatPersonWidget::showRecentlyChatMsg 查询显示最近的几条信息
 * @param count 信息数目
 */
void ChatPersonWidget::showRecentlyChatMsg(uint count)
{
     MQ_D(ChatPersonWidget);

    uint start = 0;
    //TODO LYS-获取单聊信息记录列表中当前联系人的聊天信息起始位置
    RSingleton<ChatMsgProcess>::instance()->appendC2CQueryTask(d->m_userInfo.accountId,start,count);
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

void ChatPersonWidget::resizeEvent(QResizeEvent *)
{
    MQ_D(ChatPersonWidget);

    d->windowToolBar->setGeometry(WINDOW_MARGIN_SIZE,0,width() - 3 * WINDOW_MARGIN_SIZE,Constant::TOOL_WIDTH);
}

/*!
 * @brief ChatPersonWidget::shakeWindow 抖动窗口
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
 * @brief ChatPersonWidget::queryRecordReady 响应查询聊天记录结果
 */
void ChatPersonWidget::queryRecordReady(ChatInfoUnitList &historyMsgs)
{
    foreach(ChatInfoUnit unit,historyMsgs)
    {
        emit sendQueryRecord(unit);
    }
}

/*!
 * @brief ChatPersonWidget::switchWindowSize 切换窗口尺寸
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
