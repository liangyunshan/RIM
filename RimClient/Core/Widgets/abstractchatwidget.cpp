#include "abstractchatwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QMenu>
#include <QStyle>
#include <QAction>
#include <QTimer>
#include <QKeySequence>
#include <QDateTime>
#include <QFontDialog>
#include <QColorDialog>
#include <QRgb>
#include <QClipboard>
#include <QProcess>
#include <QMimeData>
#include <QPalette>
#include <QPropertyAnimation>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#include <QImage>
#include <QDir>

#include "head.h"
#include "global.h"
#include "datastruct.h"
#include "constants.h"
#include "toolbar.h"
#include "Util/rutil.h"
#include "widget/rlabel.h"
#include "widget/rbutton.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "Widgets/textedit/complextextedit.h"
#include "Widgets/textedit/simpletextedit.h"
#include "Widgets/maindialog.h"
#include "Network/msgwrap/wrapfactory.h"
#include "others/msgqueuemanager.h"
#include "thread/filerecvtask.h"
#include "Network/netconnector.h"
#include "messdiapatch.h"
#include "previewpage.h"
#include "document.h"

#include "actionmanager/actionmanager.h"
#include "toolbar.h"
#include "thread/databasethread.h"
#include "sql/databasemanager.h"
#include "sql/sqlprocess.h"
#include "json/jsonresolver.h"
#include "screenshot.h"
#include "user/user.h"
#include "chataudioarea.h"
#include "media/audioinput.h"
#include "media/audiooutput.h"
#include "widget/rmessagebox.h"
#include "thread/chatmsgprocess.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define CHAT_MIN_WIDTH 450
#define CHAT_MIN_HEIGHT 500
#define CHAT_RIGHT_EXTREA_WIDTH   370       //消息记录等宽度
#define CHAT_SHAKE_DURATION 200             //窗口抖动动画持续时间
#define CHAT_SHAKE_LOOPCOUNT 2              //窗口抖动动画循环次数
#define CHAT_SHAKE_RANGE 6                  //窗口抖动幅度
#define SHOTIMAGE_WIDTH 80                  //截图显示宽度
#define SHOTIMAGE_HEIGHT 80                 //截图显示高度
#define TIMESTAMP_GAP 60                    //显示新时间戳的消息时间间隔

class AbstractChatWidgetPrivate : public GlobalData<AbstractChatWidget>
{
    Q_DECLARE_PUBLIC(AbstractChatWidget)

protected:
    AbstractChatWidgetPrivate(AbstractChatWidget * q):
        q_ptr(q)
    {
        initWidget();
        windowId = RUtil::UUID();
        isMaxSize = false;
        p_shotProcess = NULL;
        p_shotTimer = NULL;
        b_isScreeHide = false;
        m_content.setUi(q); //LYS
    }

    void initWidget();

    AbstractChatWidget * q_ptr;

    QWidget * contentWidget;

    QWidget * userInfoWidget;              //用户、群组头像等信息页面
    RIconLabel * userInfo_IconLabel;       /*!< 聊天头像 */
    QLabel * userInfo_NameLabel;           /*!< 对方名称 */

    ToolBar * toolBar;                     //工具栏

    QSplitter * chatSplitter;
    QWidget * bodyWidget;
    QWidget * leftWidget;
    QWidget * rightWidget;                 //右侧(历史消息、文件传输等)

    QWidget * chatWidget;                  //聊天区域
    QWebEngineView * view;                 //加载html视图
    PreviewPage *page;  //LYS
    Document m_content; //LYS
    ChatAudioArea * chatAudioArea;         //录音工具栏
    ToolBar * chatToolBar;                 //聊天工具栏
    SimpleTextEdit * chatInputArea;        //信息输入框
    QWidget * buttonWidget;                //关闭、发送等按钮栏

    RToolButton * recordButt;
    ToolBar * windowToolBar;

    bool isMaxSize;
    QRect originRect;                      //原始位置及尺寸

    SimpleUserInfo userInfo;               /*!< 当前聊天对象基本信息 */
    QString windowId;                      /*!< 窗口身份ID，只在创建时指定，可用于身份判断 */
    QProcess *p_shotProcess;
    QTimer *p_shotTimer;
    bool b_isScreeHide;
    QDateTime m_preMsgTime;
#ifdef __LOCAL_CONTACT__
    ParameterSettings::OuterNetConfig netconfig;    /*!< 当前节点网络描述信息 */
#endif
};

#define CHAT_TOOL_HEIGHT 30
#define CHAT_USER_ICON_SIZE 30

void AbstractChatWidgetPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("AbstractChatWidget");
    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    /**********用户信息***************/
    userInfoWidget = new QWidget(contentWidget);
    userInfoWidget->setFixedHeight(50);

    QHBoxLayout * userLayout = new QHBoxLayout;
    userLayout->setContentsMargins(6,0,0,0);
    userLayout->setSpacing(5);
    userInfo_IconLabel = new RIconLabel(userInfoWidget);
    userInfo_IconLabel->setTransparency(true);
    userInfo_IconLabel->setFixedSize(CHAT_USER_ICON_SIZE,CHAT_USER_ICON_SIZE);

    userInfo_NameLabel = new QLabel();
    userInfo_NameLabel->setObjectName("Chat_User_NameLabel");
    userInfo_NameLabel->setFixedHeight(CHAT_USER_ICON_SIZE);

    userLayout->addWidget(userInfo_IconLabel);
    userLayout->addWidget(userInfo_NameLabel);
    userLayout->addStretch(1);

    QVBoxLayout * userInfoLayout = new QVBoxLayout;
    userInfoLayout->setContentsMargins(0,0,0,0);
    userInfoLayout->setSpacing(0);

    userInfoLayout->addStretch(1);
    userInfoLayout->addLayout(userLayout);
    userInfoLayout->addStretch(1);
    userInfoWidget->setLayout(userInfoLayout);

    /**********工具栏***************/
    toolBar = new ToolBar(contentWidget);
    toolBar->setFixedHeight(CHAT_TOOL_HEIGHT);

    RToolButton * callButton = new RToolButton();
    callButton->setObjectName(Constant::Tool_Chat_Call);
    callButton->setFixedSize(28,24);

    toolBar->appendToolButton(callButton);
    toolBar->addStretch(1);

    QHBoxLayout * bodyLayout = new QHBoxLayout();
    bodyLayout->setContentsMargins(0,0,0,0);
    bodyLayout->setSpacing(0);

    bodyWidget = new QWidget(contentWidget);

    leftWidget = new QWidget(contentWidget);
    leftWidget->setMinimumWidth(CHAT_MIN_WIDTH);
    leftWidget->setObjectName("Chat_Left_Widget");
    leftWidget->setProperty("SideBarVisible",false);

    rightWidget = new QWidget(contentWidget);
    rightWidget->setObjectName("Chat_Right_Widget");
    rightWidget->setProperty("SideBarVisible",false);
    rightWidget->setFixedWidth(CHAT_RIGHT_EXTREA_WIDTH);

    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setSpacing(0);

    /**********聊天对话框***************/
    chatWidget = new QWidget(leftWidget);

    PreviewPage *page = new PreviewPage(q_ptr);
    view = new QWebEngineView(chatWidget);
    view->setStyleSheet("background-color:red");
    view->setPage(page);
    QObject::connect(view,SIGNAL(loadFinished(bool)),q_ptr,SLOT(finishLoadHTML(bool)));

    //LYS
    QWebChannel *channel = new QWebChannel(q_ptr);
    channel->registerObject(QStringLiteral("content"),&m_content);
    page->setWebChannel(channel);

    chatAudioArea = new ChatAudioArea(chatWidget);
    chatAudioArea->setVisible(false);
    QObject::connect(chatAudioArea,SIGNAL(prepareSendAudio()),q_ptr,SLOT(prepareSendAudio()));
    QObject::connect(chatAudioArea,SIGNAL(preapreCancelAudio()),q_ptr,SLOT(preapreCancelAudio()));

    QVBoxLayout * webLayout =  new QVBoxLayout;
    webLayout->addWidget(view);
    webLayout->addWidget(chatAudioArea);
    chatWidget->setLayout(webLayout);

    QWidget * chatInputContainter = new QWidget(leftWidget);
    chatInputContainter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    chatInputContainter->setMinimumHeight(180);//minimum chat area height
    QVBoxLayout *chatLayout = new QVBoxLayout;
    chatLayout->setContentsMargins(0,0,0,0);
    chatLayout->setSpacing(0);

    /**********聊天工具栏***************/
    chatToolBar = new ToolBar(chatInputContainter);
    chatToolBar->setContentsMargins(5,0,5,0);
    chatToolBar->setFixedHeight(CHAT_TOOL_HEIGHT);
    chatToolBar->setSpacing(5);
    chatToolBar->setIconSize(QSize(CHAT_TOOL_HEIGHT,CHAT_TOOL_HEIGHT));

    RToolButton * fontButt = new RToolButton();
    fontButt->setObjectName(Constant::Tool_Chat_Font);
    fontButt->setToolTip(QObject::tr("Font"));
    //ShangChao
    QObject::connect(fontButt,SIGNAL(clicked(bool)),q_ptr,SLOT(slot_SetChatEditFont(bool)));

    RToolButton * fontColorButt = new RToolButton();
    fontColorButt->setObjectName(Constant::Tool_Chat_FontColor);
    fontColorButt->setToolTip(QObject::tr("FontColor"));
    QObject::connect(fontColorButt,SIGNAL(clicked(bool)),q_ptr,SLOT(slot_SetChatEditFontColor(bool)));

    RToolButton * faceButt = new RToolButton();
    faceButt->setObjectName(Constant::Tool_Chat_Face);
    faceButt->setToolTip(QObject::tr("Emoji"));

    RToolButton * shakeButt = new RToolButton();
    shakeButt->setObjectName(Constant::Tool_Chat_Shake);
    shakeButt->setToolTip(QObject::tr("Shake window"));
    QObject::connect(shakeButt,SIGNAL(clicked(bool)),q_ptr,SLOT(slot_ShakeWidget(bool)));

    RToolButton * imageButt = new RToolButton();
    imageButt->setObjectName(Constant::Tool_Chat_Image);
    imageButt->setToolTip(QObject::tr("Image"));

    RToolButton * screenShotButt = new RToolButton();
    screenShotButt->setObjectName(Constant::Tool_Chat_ScreenShot);
    screenShotButt->setToolTip(QObject::tr("Screenshot"));
    screenShotButt->setPopupMode(QToolButton::MenuButtonPopup);
    QObject::connect(screenShotButt,SIGNAL(clicked()),G_pScreenShotAction,SIGNAL(triggered()));

    RToolButton * audioButt = new RToolButton();
    audioButt->setObjectName(Constant::Tool_Chat_Audio);
    audioButt->setToolTip(QObject::tr("Audio"));
    QObject::connect(audioButt,SIGNAL(clicked(bool)),q_ptr,SLOT(respShowAudioArea(bool)));

    QMenu * screenShotMenu = new QMenu(q_ptr);
    screenShotMenu->setObjectName(this->windowId + "ScreenShotMenu");

    QAction * hideWindowAction = new QAction(QObject::tr("Hide current window while screenshot "),q_ptr);
    hideWindowAction->setObjectName(this->windowId + "HideScreenshot");
    hideWindowAction->setCheckable(true);
    hideWindowAction->setChecked(false);
    QObject::connect(hideWindowAction,SIGNAL(triggered(bool)),q_ptr,SLOT(slot_ScreenShotHide(bool)));

    screenShotMenu->addAction(G_pScreenShotAction);
    screenShotMenu->addAction(hideWindowAction);

    screenShotButt->setMenu(screenShotMenu);

    recordButt = new RToolButton();
    recordButt->setObjectName(Constant::Tool_Chat_Record);
    recordButt->setToolTip(QObject::tr("Record data"));
    recordButt->removeIcon();
    recordButt->setText(QObject::tr("Record data"));
    recordButt->setCheckable(true);
    recordButt->setFixedWidth(recordButt->fontMetrics().width(recordButt->text()) + 10);
    QObject::connect(recordButt,SIGNAL(toggled(bool)),q_ptr,SLOT(setSideVisible(bool)));

    chatToolBar->appendToolButton(fontButt);
    chatToolBar->appendToolButton(fontColorButt);
    chatToolBar->appendToolButton(faceButt);
    chatToolBar->appendToolButton(shakeButt);
    chatToolBar->appendToolButton(imageButt);
    chatToolBar->appendToolButton(screenShotButt);
    chatToolBar->appendToolButton(audioButt);
    chatToolBar->addStretch(1);
    chatToolBar->appendToolButton(recordButt);

    QObject::connect(G_pScreenShot,SIGNAL(sig_ShotReady(bool)),q_ptr,SLOT(slot_ScreenShot_Ready(bool)));

    /**********聊天内容输入框***************/
    chatInputArea = new SimpleTextEdit(chatInputContainter);
    chatInputArea->setFocus();

    /**********底部按钮区***************/
    buttonWidget = new QWidget(chatInputContainter);
    buttonWidget->setFixedHeight(35);

    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0,0,10,10);

    RButton * closeButton = new RButton(buttonWidget);
    closeButton->setObjectName(Constant::Button_Chat_Close_Window);
    closeButton->setShortcut(ActionManager::instance()->shortcut(Constant::Button_Chat_Close_Window,QKeySequence("Alt+C")));
    closeButton->setText(QObject::tr("Close window"));
    QObject::connect(closeButton,SIGNAL(clicked()),q_ptr,SLOT(close()));

    RButton * sendMessButton = new RButton(buttonWidget);
    sendMessButton->setObjectName(Constant::Button_Chat_Send);
    sendMessButton->setText(QObject::tr("Send message"));
    QObject::connect(sendMessButton,SIGNAL(clicked(bool)),q_ptr,SLOT(slot_ButtClick_SendMsg(bool)));
    QObject::connect(chatInputArea,SIGNAL(sigEnter()),q_ptr,SLOT(slot_CheckSendEnter()));

    RToolButton * extralButton = new RToolButton();
    extralButton->setObjectName(Constant::Tool_Chat_SendMess);

    QHBoxLayout * composeLayout = new QHBoxLayout();
    composeLayout->setSpacing(0);
    composeLayout->setContentsMargins(0,0,0,0);

    composeLayout->addWidget(sendMessButton);
    composeLayout->addWidget(extralButton);

    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addLayout(composeLayout);

    buttonWidget->setLayout(buttonLayout);

    chatLayout->addWidget(chatToolBar);
    chatLayout->addWidget(chatInputArea);
    chatLayout->addWidget(buttonWidget);
    chatInputContainter->setLayout(chatLayout);

    chatSplitter = new QSplitter(Qt::Vertical);
    chatSplitter->addWidget(chatWidget);
    chatSplitter->addWidget(chatInputContainter);
    chatSplitter->setStretchFactor(0,5);
    chatSplitter->setStretchFactor(1,1);

    leftLayout->addWidget(chatSplitter);

    leftWidget->setLayout(leftLayout);

    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->setSpacing(0);
    rightWidget->setLayout(rightLayout);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    layout->addWidget(leftWidget);
    layout->addWidget(rightWidget);

    bodyLayout->addLayout(layout);
    bodyWidget->setLayout(bodyLayout);

    contentLayout->addWidget(userInfoWidget);
    contentLayout->addWidget(toolBar);
    contentLayout->addWidget(bodyWidget);

    contentWidget->setLayout(contentLayout);
    q_ptr->setContentWidget(contentWidget);

    /**********窗口控制区***************/
    windowToolBar = new ToolBar(contentWidget);
    windowToolBar->setContentsMargins(5,0,0,0);

    windowToolBar->setToolFlags(ToolBar::TOOL_ACTION);
    QObject::connect(windowToolBar,SIGNAL(minimumWindow()),q_ptr,SLOT(showMinimized()));
    QObject::connect(windowToolBar,SIGNAL(closeWindow()),q_ptr,SLOT(close()));
}

#define WINDOW_SIZ_WIDTH_FACTOR 0.4
#define WINDOW_SIZ_HEIGHT_FACTOR 0.80

AbstractChatWidget::AbstractChatWidget(QWidget *parent):
    d_ptr(new AbstractChatWidgetPrivate(this)),
    Widget(parent)
{
    setMinimumHeight(CHAT_MIN_HEIGHT);
    setWindowFlags(windowFlags() &(~Qt::Tool));

    d_ptr->userInfoWidget->installEventFilter(this);
    d_ptr->windowToolBar->installEventFilter(this);

    d_ptr->view->load(QUrl("qrc:/html/resource/html/chatRecord.html"));
    d_ptr->view->show();

    QTimer::singleShot(0,this,SLOT(resizeOnce()));
    RSingleton<Subject>::instance()->attach(this);
    connect(this,SIGNAL(maxWindow(bool)),this,SLOT(noticeWebViewShift(bool)));
}

AbstractChatWidget::~AbstractChatWidget()
{
    MQ_D(AbstractChatWidget);
    if(d->p_shotProcess)
    {
        delete d->p_shotProcess;
        d->p_shotProcess = NULL;
    }
    if(d->p_shotTimer)
    {
        delete d->p_shotTimer;
        d->p_shotTimer = NULL;
    }
    d->chatWidget->deleteLater();

    delete d;
}

QString AbstractChatWidget::widgetId()
{
    MQ_D(AbstractChatWidget);
    return d->windowId;
}

void AbstractChatWidget::showRecentlyChatMsg(uint count)
{
    MQ_D(AbstractChatWidget);
    uint start = 0;
    //TODO LYS-获取单聊信息记录列表中当前联系人的聊天信息起始位置
    RSingleton<ChatMsgProcess>::instance()->appendC2CQueryTask(d->userInfo.accountId,start,count);
}

void AbstractChatWidget::setUserInfo(SimpleUserInfo info)
{
    MQ_D(AbstractChatWidget);
    d->userInfo = info;
    d->userInfo_NameLabel->setText(info.nickName);
    d->userInfo_IconLabel->setPixmap(G_User->getIcon(d->userInfo.isSystemIcon,d->userInfo.iconId));
    setWindowIcon(RSingleton<ImageManager>::instance()->getCircularIcons(G_User->getIcon(info.isSystemIcon,info.iconId)));
    setWindowTitle(info.nickName);
}

#ifdef __LOCAL_CONTACT__
/*!
 * @brief 设置节点外发信息配置
 * @param[in] config 节点外发配置信息
 * @note 该配置信息描述了与当前节点通信的网络描述信息，包括使用的通信方式，报文格式等。
 */
void AbstractChatWidget::setOuterNetConfig(const ParameterSettings::OuterNetConfig &config)
{
    MQ_D(AbstractChatWidget);
    d->netconfig = config;
}
#endif

void AbstractChatWidget::initChatRecord()
{
    ChatMsgProcess *chatProcess = RSingleton<ChatMsgProcess>::instance();
//    connect(chatProcess,SIGNAL(C2CResultReady(ChatInfoUnitList &)),
//            this,SLOT());
    connect(chatProcess,SIGNAL(finished()),
            chatProcess,SLOT(deleteLater()));
    //showRecentlyChatMsg(3);
}

void AbstractChatWidget::onMessage(MessageType type)
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

//窗口抖动
void AbstractChatWidget::shakeWindow()
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
 * \brief AbstractChatWidget::appendRecvMsg 直接将接收到的消息数据显示在聊天对话框中
 * \param recvMsg 收到的消息数据
 */
void AbstractChatWidget::appendRecvMsg(TextRequest recvMsg)
{
    MQ_D(AbstractChatWidget);
    QString t_showMsgScript = QString("");
    QDateTime t_epochTime(QDate(1970,1,1),QTime(0,0,0));
    QDateTime t_curMsgTime = t_epochTime.addMSecs(recvMsg.timeStamp);
    QString t_localHtml = recvMsg.sendData;
    QString t_headPath = QString("");

    if(d->m_preMsgTime.isNull())
    {
        d->m_preMsgTime = t_curMsgTime;
        if(d->m_preMsgTime.date() != G_loginTime.date())
        {
            appendChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(G_loginTime.time().secsTo(d->m_preMsgTime.time())>= TIMESTAMP_GAP)
        {
            appendChatTimeNote(t_curMsgTime,TIME);
        }
    }
    else
    {
        if(t_curMsgTime.date() != d->m_preMsgTime.date())
        {
            appendChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(d->m_preMsgTime.time().secsTo(t_curMsgTime.time()) >= TIMESTAMP_GAP)
        {
            appendChatTimeNote(t_curMsgTime,TIME);
        }
        d->m_preMsgTime = t_curMsgTime;
    }

    RUtil::removeEccapeDoubleQuote(t_localHtml);
    RUtil::setAbsoulteImgPath(t_localHtml,G_User->BaseInfo().accountId);

    t_headPath = G_User->getIconAbsoultePath(d->userInfo.isSystemIcon,d->userInfo.iconId);
    t_showMsgScript = QString("appendMesRecord(%1,'%2','%3')").arg(RECV).arg(t_localHtml).arg(t_headPath);

    d->view->page()->runJavaScript(t_showMsgScript);
}

/*!
 * @brief AbstractChatWidget::playVoiceMessage 播放语音
 * @param path 语音文件路径
 */
void AbstractChatWidget::playVoiceMessage(QString audioName)
{
    RSingleton<AudioOutput>::instance()->setAudioSaveDir(G_User->getC2CAudioPath());
    RSingleton<AudioOutput>::instance()->start(audioName);
}

//更新快捷键
void AbstractChatWidget::slot_UpdateKeySequence()
{

}

//响应聊天框的历史信息查询
void AbstractChatWidget::slot_QueryHistoryRecords(int user_query_id, int currStartRow)
{
    Q_UNUSED(user_query_id);
    Q_UNUSED(currStartRow);
    //TODO LYS-实现查询历史消息记录
}

void AbstractChatWidget::resizeOnce()
{
    QSize size = qApp->desktop()->screen()->size();

    int w = size.width() * WINDOW_SIZ_WIDTH_FACTOR;
    int h = size.height() * WINDOW_SIZ_HEIGHT_FACTOR;

    setGeometry((size.width() - w)/2,(size.height() - h)/2,w,h);

    setSideVisible(false);
}

//控制历史消息记录窗口弹出与隐藏
void AbstractChatWidget::setSideVisible(bool flag)
{
    MQ_D(AbstractChatWidget);

    d->leftWidget->setProperty("SideBarVisible",flag);
    d->rightWidget->setProperty("SideBarVisible",flag);

    if(flag)
    {
        resize(d->chatWidget->width() + CHAT_RIGHT_EXTREA_WIDTH + WINDOW_MARGIN_SIZE * 2,height());
    }
    else
    {
        int tmpWidth = d->chatWidget->width() + 2 * WINDOW_MARGIN_SIZE;
        resize(tmpWidth < minimumWidth()?minimumWidth():tmpWidth,height());
        d->recordButt->setChecked(false);
    }

    d->rightWidget->setVisible(flag);
    repolish(d->leftWidget);
    repolish(d->rightWidget);
}

//出现字体设置界面
void AbstractChatWidget::slot_SetChatEditFont(bool flag)
{
    Q_UNUSED(flag);
    MQ_D(AbstractChatWidget);
    bool ok;
    QFont font = QFontDialog::getFont(
                  &ok, QFont("Helvetica [Cronyx]", 10), this);
    if(ok)
    {
        d->chatInputArea->setInputFont(font);
    }
    else
    {

    }
}

//出现字体颜色设置界面
void AbstractChatWidget::slot_SetChatEditFontColor(bool flag)
{
    Q_UNUSED(flag);
    MQ_D(AbstractChatWidget);
    QColor color = QColorDialog::getColor(Qt::white, this, QObject::tr("ColorDialog"));
    d->chatInputArea->setInputColor(color);
}

//实现窗口抖动
//在接收到其它人发送的抖动时，可调用该方法实现
void AbstractChatWidget::slot_ShakeWidget(bool flag)
{
    Q_UNUSED(flag)
    MQ_D(AbstractChatWidget);
    TextRequest * request = new TextRequest;
    request->msgCommand = MSG_TEXT_SHAKE;
    request->otherSideId = d->userInfo.accountId;
    request->accountId = G_User->BaseInfo().accountId;
    request->timeStamp = RUtil::timeStamp();
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);

    shakeWindow();
}

//截图完成
void AbstractChatWidget::slot_ScreenShot_Ready(bool)
{
    MQ_D(AbstractChatWidget);
    if(!this->isActiveWindow())
    {
        return ;
    }
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if(mimeData->hasImage())
    {
        d->chatInputArea->insertCopyImage(clipboard->image());
    }
    if(this->isMinimized())
    {
        this->showNormal();
        this->raise();
        this->activateWindow();
    }
}

//截屏时聊天界面是否隐藏
void AbstractChatWidget::slot_ScreenShotHide(bool flag)
{
    MQ_D(AbstractChatWidget);
    d->b_isScreeHide = flag;
}

void AbstractChatWidget::slot_ScreenTimeout()
{
    MQ_D(AbstractChatWidget);
    d->p_shotTimer->stop();
    if(d->p_shotProcess)
    {
        d->p_shotProcess->start( "rundll32 "+ QApplication::applicationDirPath()+ "/PrScrn.dll PrScrn" );
    }
}

//判断是否Enter发送内容
void AbstractChatWidget::slot_CheckSendEnter()
{
    slot_ButtClick_SendMsg(true);
}

/*!
 * @brief 发送信息
 * @details 将用户信息输入区的内容封装成消息请求加入发送队列，同时将信息保存至消息队列用于后期的管理
 * @param[in] flag 未使用
 * @return 无
 */
void AbstractChatWidget::slot_ButtClick_SendMsg(bool flag)
{
    Q_UNUSED(flag)
    MQ_D(AbstractChatWidget);

//test
    //TODO 20180423 向历史会话记录列表插入一条记录
    HistoryChatRecord record;
    record.accountId = d->userInfo.accountId;
    record.nickName = d->userInfo.nickName;
    record.dtime = RUtil::currentMSecsSinceEpoch();
    record.lastRecord = RUtil::getTimeStamp();
    record.systemIon = d->userInfo.isSystemIcon;
    record.iconId = d->userInfo.iconId;
    record.type = CHAT_C2C;
    record.status = d->userInfo.status;
    MessDiapatch::instance()->onAddHistoryItem(record);
//test

    QString t_simpleHtml = QString("");
    d->chatInputArea->extractPureHtml(t_simpleHtml);

    QString t_localHtml = t_simpleHtml;
    ChatInfoUnit t_unit;
    t_unit.contentType = MSG_TEXT_TEXT;
    t_unit.accountId = d->userInfo.accountId;
    t_unit.nickName = d->userInfo.nickName;
    t_unit.dtime = RUtil::currentMSecsSinceEpoch();
    t_unit.contents = t_localHtml;
    appendChatRecord(SEND,t_unit);

    //转义原始Html
    QString t_sendHtml = t_simpleHtml;
    RUtil::escapeSingleQuote(t_sendHtml);
    RUtil::escapeDoubleQuote(t_sendHtml);

    //存储发送信息到数据库
    t_unit.contents = t_sendHtml;   //将转义处理后的内容存储到数据库中
    RSingleton<ChatMsgProcess>::instance()->appendC2CStoreTask(t_unit);//加载第二条的时候就有问题了

    //发送Html内容给联系人
    TextRequest * request = new TextRequest;
    request->type = OperatePerson;
    request->textId = RUtil::UUID();

    if(G_User->systemSettings()->encryptionCheck){
        //TODO 数据加密
    }
    request->isEncryption = G_User->systemSettings()->encryptionCheck;

    if(G_User->systemSettings()->compressCheck){
        //TODO 数据压缩
    }
    request->isCompress = G_User->systemSettings()->compressCheck;

    request->textType = TEXT_NORAML;
#ifdef __LOCAL_CONTACT__
    request->otherSideId = d->netconfig.nodeId;
#else
    request->otherSideId = d->userInfo.accountId;
#endif
    request->accountId = G_User->BaseInfo().accountId;
    request->sendData = t_sendHtml;
    request->timeStamp = RUtil::timeStamp();

#ifdef __LOCAL_CONTACT__
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request,d->netconfig.communicationMethod,d->netconfig.messageFormat);
#else
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
#endif
    RSingleton<MsgQueueManager>::instance()->enqueue(request);

    //分开发送输入框中的图片
//    QStringList t_imgDirs;
//    d->chatInputArea->getInputedImgs(t_imgDirs);
//    if(!t_imgDirs.isEmpty())
//    {
//        for(int imgIndex=0;imgIndex<t_imgDirs.count();imgIndex++)
//        {
//            QString t_imgPath = t_imgDirs.at(imgIndex);

//            QString fileName = t_imgPath;
//            FileItemDesc * desc = new FileItemDesc;
//            desc->id = RUtil::UUID();
//            desc->fullPath = fileName;
//            desc->fileSize = QFileInfo(fileName).size();
//            desc->otherSideId = d->userInfo.accountId;
//            desc->itemType = FILE_ITEM_CHAT_UP;
//            desc->itemKind = FILE_IMAGE;
//            FileRecvTask::instance()->addSendItem(desc);

//            Q_UNUSED(t_imgPath);
//        }
//    }

    d->chatInputArea->clear();
}

//响应数据库线程查询结果
void AbstractChatWidget::slot_DatabaseThread_ResultReady(int id, ChatInfoUnitList list)
{
    if(id == 0)
    {
        foreach(ChatInfoUnit unit,list)
        {
            //获取收到的Html消息,进行处理后显示在聊天记录中
            appendChatRecord(RECV,unit);
        }
    }

}

//QWebEngineView加载HTML文件完成
void AbstractChatWidget::finishLoadHTML(bool)
{
    qDebug()<<"finishLoadHTML";
    setFontIconFilePath();
}

/*!
 * \brief AbstractChatWidget::noticeWebViewShift 最大化或者还原WebView
 */
void AbstractChatWidget::noticeWebViewShift(bool)
{
    MQ_D(AbstractChatWidget);
    QString t_shiftScript = QString("toggleWindow()");
    d->view->page()->runJavaScript(t_shiftScript);
}

/*!
 * @brief 显示录音区域
 * @details 若其它窗口未在录音则直接录音；
 *          若其它窗口已经在录音，则提示，避免再打开录音功能
 */
void AbstractChatWidget::respShowAudioArea(bool)
{
    MQ_D(AbstractChatWidget);

    if(RSingleton<AudioInput>::instance()->isRecording()){
        RMessageBox::warning(this,tr("warning"),tr("Other window is recording,please try later!"),RMessageBox::Yes);
        return;
    }

    RSingleton<AudioInput>::instance()->setAudioSaveDir(G_User->getC2CAudioPath());

    RSingleton<AudioInput>::instance()->start();
    d->chatAudioArea->setVisible(true);
    d->chatAudioArea->start();
}

/*!
 * @brief 准备发送录音文件
 */
void AbstractChatWidget::prepareSendAudio()
{
    MQ_D(AbstractChatWidget);
    if(RSingleton<AudioInput>::instance()->stop()){
       QString lastRecordFileFullName = RSingleton<AudioInput>::instance()->lastRecordFullPath();

       //【聊天文件发送:1/5】向服务器发送文件
       if(G_User->isFileOnLine()){
           FileItemDesc * desc = new FileItemDesc;
           desc->id = RUtil::UUID();
           desc->fullPath = lastRecordFileFullName;
           desc->fileSize = QFileInfo(lastRecordFileFullName).size();
           desc->itemType = FILE_ITEM_CHAT_UP;
           desc->itemKind = FILE_AUDIO;
           desc->otherSideId = d->userInfo.accountId;

           FileRecvTask::instance()->addSendItem(desc);

           appendVoiceMsg(SEND,QFileInfo(lastRecordFileFullName).fileName());
       }
    }
    d->chatAudioArea->setVisible(false);
}

/*!
 * @brief 取消录音文件发送
 * @details 1.将已经生成的文件删除
 *          2.隐藏录音区域
 */
void AbstractChatWidget::preapreCancelAudio()
{
    MQ_D(AbstractChatWidget);
    if(RSingleton<AudioInput>::instance()->stop()){

        RSingleton<AudioInput>::instance()->clear();
    }
    d->chatAudioArea->setVisible(false);
}

bool AbstractChatWidget::eventFilter(QObject *watched, QEvent *event)
{
    MQ_D(AbstractChatWidget);
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

void AbstractChatWidget::resizeEvent(QResizeEvent *)
{
    MQ_D(AbstractChatWidget);
    d->windowToolBar->setGeometry(WINDOW_MARGIN_SIZE,0,width() - 3 * WINDOW_MARGIN_SIZE,Constant::TOOL_WIDTH);
}

void AbstractChatWidget::switchWindowSize()
{
    MQ_D(AbstractChatWidget);

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
//      ShowWindow(hd, SW_MAXIMIZE );
    }
#endif
    d->isMaxSize = !d->isMaxSize;

    setShadowWindow(!isMaximized());
}

/*!
 * \brief AbstractChatWidget::appendChatRecord 将信息追加显示在聊天信息记录界面上
 * \param html 收到的html格式的聊天信息
 */
void AbstractChatWidget::appendChatRecord(MsgTarget source, const ChatInfoUnit &unitMsg)
{
    MQ_D(AbstractChatWidget);
    QString t_showMsgScript = QString("");
    QString t_localHtml = unitMsg.contents;
    QDateTime t_curMsgTime = RUtil::addMSecsToEpoch(unitMsg.dtime);
    QString t_headPath = QString("");

    if(d->m_preMsgTime.isNull())
    {
        d->m_preMsgTime = t_curMsgTime;
        if(d->m_preMsgTime.date() != G_loginTime.date())
        {
            appendChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(G_loginTime.time().secsTo(d->m_preMsgTime.time())>= TIMESTAMP_GAP)
        {
            appendChatTimeNote(t_curMsgTime,TIME);
        }
    }
    else
    {
        if(t_curMsgTime.date() != d->m_preMsgTime.date())
        {
            appendChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(d->m_preMsgTime.time().secsTo(t_curMsgTime.time()) >= TIMESTAMP_GAP)
        {
            appendChatTimeNote(t_curMsgTime,TIME);
        }
        d->m_preMsgTime = t_curMsgTime;
    }

    RUtil::setAbsoulteImgPath(t_localHtml,G_User->BaseInfo().accountId);
    RUtil::escapeSingleQuote(t_localHtml);

    if(source == RECV)
    {
        User tmpUser(d->userInfo.accountId);
        t_headPath = tmpUser.getIconAbsoultePath(false,d->userInfo.iconId);
    }
    else
    {
        t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
    }
    t_showMsgScript = QString("appendMesRecord(%1,'%2','%3')").arg(source).arg(t_localHtml).arg(t_headPath);

    d->view->page()->runJavaScript(t_showMsgScript);
}

/*!
 * @brief AbstractChatWidget::setFontIconFilePath 修改HTML中字体图标文件链接路径
 */
void AbstractChatWidget::setFontIconFilePath()
{
    MQ_D(AbstractChatWidget);
    QString t_currentPath = QDir::currentPath();
    t_currentPath = QDir::fromNativeSeparators(t_currentPath);
    QString t_setLinkFontHerfScript = QString("setAbsoulteFontIconHerf('%1')").arg(t_currentPath);
    d->view->page()->runJavaScript(t_setLinkFontHerfScript);
}

/*!
 * @brief AbstractChatWidget::appendChatTimeNote 显示时间提示信息
 * @param format 时间显示格式
 * @param content 显示内容
 */
void AbstractChatWidget::appendChatTimeNote(QDateTime content,TimeFormat format)
{
    MQ_D(AbstractChatWidget);
    QString t_curMsgTime = QString("");
    QString t_showTimeScript = QString("");
    if(content.isNull())
    {
        return;
    }
    switch (format) {
    case TIME:
        t_curMsgTime = content.time().toString("h:mm:ss");
        break;
    case DATETIME:
        t_curMsgTime = content.toString("yyyy/M/d h:mm:ss");
        break;
    default:
        break;
    }
    t_showTimeScript = QString("showMessageTime('%1')").arg(t_curMsgTime);
    d->view->page()->runJavaScript(t_showTimeScript);
}

/*!
 * @brief AbstractChatWidget::appendChatNotice 显示操作提示信息
 * @param type  信息类型
 * @param content 信息内容
 */
void AbstractChatWidget::appendChatNotice(QString content,NoticeType type)
{
    MQ_D(AbstractChatWidget);
    if(content.isEmpty())
    {
        return;
    }
    QString t_showNotice = QString("setNotice(%1,'%2')").arg(type).arg(content);
    d->view->page()->runJavaScript(t_showNotice);
}

/*!
 * @brief AbstractChatWidget::appendVoiceMsg 显示语音消息
 */
void AbstractChatWidget::appendVoiceMsg(MsgTarget source,QString recordFileName)
{
    MQ_D(AbstractChatWidget);

    QString t_headPath;
    if(source == SEND){
        t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
    }else if(source == RECV){
        t_headPath = G_User->getIconAbsoultePath(d->userInfo.isSystemIcon,d->userInfo.iconId);
    }

    QString t_showVoiceMsgScript = QString("appendVoiceMsg(%1,'%2','%3','%4')").arg(source).arg(50).arg(recordFileName).arg(t_headPath);
    d->view->page()->runJavaScript(t_showVoiceMsgScript);
}
