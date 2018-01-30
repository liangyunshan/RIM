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
#include <QMimeData>

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
#include "slidebar.h"
#include "Widgets/maindialog.h"

#include "actionmanager/actionmanager.h"
#include "toolbar.h"
#include "thread/databasethread.h"
#include "sql/databasemanager.h"
#include "screenshot.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define CHAT_MIN_WIDTH 450
#define CHAT_MIN_HEIGHT 500
#define CHAT_RIGHT_EXTREA_WIDTH   370       //消息记录等宽度

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
        p_DatabaseThread = NULL;
        p_shotProcess = NULL;
        p_shotTimer = NULL;
    }

    void initWidget();

    AbstractChatWidget * q_ptr;

    QWidget * contentWidget;

    QWidget * userInfoWidget;              //用户、群组头像等信息页面
    RIconLabel * userInfo_IconLabel;
    QLabel * userInfo_NameLabel;

    ToolBar * toolBar;                     //工具栏
    SlideBar * slideBar;

    QSplitter * chatSplitter;
    QWidget * bodyWidget;
    QWidget * leftWidget;
    QWidget * rightWidget;                 //右侧

    QWidget * chatWidget;                  //聊天区域
    ComplexTextEdit * chatArea;            //对话信息区域
    ToolBar * chatToolBar;                 //聊天工具栏
    SimpleTextEdit * chatInputArea;        //信息输入框
    QWidget * buttonWidget;                //关闭、发送等按钮栏

    RToolButton * recordButt;
    ToolBar * windowToolBar;

    bool isMaxSize;
    QRect originRect;                      //原始位置及尺寸

    QString windowId;
    DatabaseThread * p_DatabaseThread;
    QProcess *p_shotProcess;
    QTimer *p_shotTimer;
    QTimer *p_shakeTimer;
    int     m_nPosition;
    QPoint  m_curPos;
    bool b_isScreeHide;
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
    userInfo_IconLabel->setPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon());
    userInfo_IconLabel->setFixedSize(CHAT_USER_ICON_SIZE,CHAT_USER_ICON_SIZE);

    userInfo_NameLabel = new QLabel();
    userInfo_NameLabel->setObjectName("Chat_User_NameLabel");
    userInfo_NameLabel->setText(QStringLiteral("我是测试对话框"));
    userInfo_NameLabel->setFixedHeight(CHAT_USER_ICON_SIZE);

    q_ptr->setWindowTitle(QStringLiteral("测试对话框"));
    q_ptr->setWindowIcon(RSingleton<ImageManager>::instance()->getCircularIcons(RSingleton<ImageManager>::instance()->getSystemUserIcon()));

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
    QHBoxLayout * tmpLayout = new QHBoxLayout();
    tmpLayout->setContentsMargins(0,0,0,0);
    tmpLayout->setSpacing(0);

    chatArea = new ComplexTextEdit(leftWidget);
    chatArea->setReadOnly(true);

    slideBar = new SlideBar(contentWidget);
    slideBar->setFixedWidth(10);

    tmpLayout->addWidget(chatArea);
    tmpLayout->addWidget(slideBar);
    chatWidget->setLayout(tmpLayout);

    QWidget * chatInputContainter = new QWidget(leftWidget);
    chatInputContainter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    chatInputContainter->setMinimumHeight(110);
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

    RToolButton * fontColorButt = new RToolButton();
    fontColorButt->setObjectName(Constant::Tool_Chat_FontColor);
    fontColorButt->setToolTip(QObject::tr("FontColor"));

    RToolButton * faceButt = new RToolButton();
    faceButt->setObjectName(Constant::Tool_Chat_Face);
    faceButt->setToolTip(QObject::tr("Emoji"));

    RToolButton * shakeButt = new RToolButton();
    shakeButt->setObjectName(Constant::Tool_Chat_Shake);
    shakeButt->setToolTip(QObject::tr("Shake window"));

    RToolButton * imageButt = new RToolButton();
    imageButt->setObjectName(Constant::Tool_Chat_Image);
    imageButt->setToolTip(QObject::tr("Image"));

    RToolButton * screenShotButt = new RToolButton();
    screenShotButt->setObjectName(Constant::Tool_Chat_ScreenShot);
    screenShotButt->setToolTip(QObject::tr("Screenshot"));
    screenShotButt->setPopupMode(QToolButton::InstantPopup);

    QMenu * screenShotMenu = new QMenu(q_ptr);
    screenShotMenu->setObjectName(this->windowId + "ScreenShotMenu");

    QAction * hideWindowAction = new QAction(QObject::tr("Hide current window while screenshot "),q_ptr);
    hideWindowAction->setObjectName(this->windowId + "HideScreenshot");
    hideWindowAction->setCheckable(true);
    hideWindowAction->setChecked(false);

    screenShotMenu->addAction(G_pScreenShotAction);
    screenShotMenu->addAction(hideWindowAction);

    screenShotButt->setMenu(screenShotMenu);

    recordButt = new RToolButton();
    recordButt->setObjectName(Constant::Tool_Chat_Record);
    recordButt->setToolTip(QObject::tr("Record data"));
    recordButt->removeIcon();
    recordButt->setText(QObject::tr("Record data"));

    chatToolBar->appendToolButton(fontButt);
    chatToolBar->appendToolButton(fontColorButt);
    chatToolBar->appendToolButton(faceButt);
    chatToolBar->appendToolButton(shakeButt);
    chatToolBar->appendToolButton(imageButt);
    chatToolBar->appendToolButton(screenShotButt);
    chatToolBar->addStretch(1);
    chatToolBar->appendToolButton(recordButt);

    recordButt->setCheckable(true);
    recordButt->setFixedWidth(recordButt->fontMetrics().width(recordButt->text()) + 10);
    QObject::connect(recordButt,SIGNAL(toggled(bool)),q_ptr,SLOT(setSideVisible(bool)));

    //ShangChao
    QObject::connect(fontButt,SIGNAL(clicked(bool)),q_ptr,SLOT(slot_SetChatEditFont(bool)));
    QObject::connect(fontColorButt,SIGNAL(clicked(bool)),q_ptr,SLOT(slot_SetChatEditFontColor(bool)));
    QObject::connect(shakeButt,SIGNAL(clicked(bool)),q_ptr,SLOT(slot_ShakeWidget(bool)));
    QObject::connect(G_pScreenShot,SIGNAL(sig_ShotReady(bool)),q_ptr,SLOT(slot_ScreenShot_Ready(bool)));
    QObject::connect(hideWindowAction,SIGNAL(triggered(bool)),q_ptr,SLOT(slot_ScreenShotHide(bool)));
    //

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
    QObject::connect(this->chatInputArea,SIGNAL(sigEnter()),q_ptr,SLOT(slot_CheckSendEnter()));

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

    QObject::connect(slideBar,SIGNAL(slideStateChanged(bool)),q_ptr,SLOT(setSideVisible(bool)));

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
    d_ptr->chatInputArea->setFocus();

    initChatRecord();

    d_ptr->p_shakeTimer = NULL;
    d_ptr->b_isScreeHide = false;

    QTimer::singleShot(0,this,SLOT(resizeOnce()));

    RSingleton<Subject>::instance()->attach(this);
}

AbstractChatWidget::~AbstractChatWidget()
{
    if(d_ptr->p_shotProcess)
    {
        delete d_ptr->p_shotProcess;
        d_ptr->p_shotProcess = NULL;
    }
    if(d_ptr->p_shotTimer)
    {
        delete d_ptr->p_shotTimer;
        d_ptr->p_shotTimer = NULL;
    }
    if(d_ptr->p_shakeTimer)
    {
        delete d_ptr->p_shakeTimer;
        d_ptr->p_shakeTimer = NULL;
    }

    delete d_ptr;
}

QString AbstractChatWidget::widgetId()
{
    MQ_D(AbstractChatWidget);
    return d->windowId;
}

void AbstractChatWidget::recvChatMsg(QByteArray msg)
{
    TextUnit::ChatInfoUnit  readJson = d_ptr->chatInputArea->ReadJSONFile(msg);
    d_ptr->chatArea->insertMeChatText(readJson);

    //TODO:将记录写入到数据库
    DatabaseManager::Instance()->insertTableUserChatInfo(readJson);
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

//更新快捷键
void AbstractChatWidget::slot_UpdateKeySequence()
{

}

void AbstractChatWidget::resizeOnce()
{
    QSize size = qApp->desktop()->screen()->size();

    int w = size.width() * WINDOW_SIZ_WIDTH_FACTOR;
    int h = size.height() * WINDOW_SIZ_HEIGHT_FACTOR;

    setGeometry((size.width() - w)/2,(size.height() - h)/2,w,h);

    setSideVisible(false);
}

void AbstractChatWidget::setSideVisible(bool flag)
{
    MQ_D(AbstractChatWidget);

    d->slideBar->setState(flag);
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
    Q_UNUSED(flag)
    bool ok;
    QFont font = QFontDialog::getFont(
                  &ok, QFont("Helvetica [Cronyx]", 10), this);
    if (ok) {
        qDebug()<<__FILE__<<__LINE__<<"\n"
               <<"font:"<<font
              <<"\n";
        d_ptr->chatInputArea->setFont(font);
    } else {

    }
}

//出现字体颜色设置界面
void AbstractChatWidget::slot_SetChatEditFontColor(bool flag)
{
    Q_UNUSED(flag)

    QColor color = QColorDialog::getColor(Qt::white, this, QObject::tr("ColorDialog"));
    d_ptr->chatInputArea->setTextColor(color);
}

//实现窗口抖动
//在接收到其它人发送的抖动时，可调用该方法实现
void AbstractChatWidget::slot_ShakeWidget(bool flag)
{
    Q_UNUSED(flag)

    if(d_ptr->p_shakeTimer==NULL)
    {
        d_ptr->p_shakeTimer = new QTimer();
        QObject::connect(d_ptr->p_shakeTimer,SIGNAL(timeout()),this,SLOT(slot_ShakeTimeout()));
        d_ptr->p_shakeTimer->setInterval(40);
    }
    d_ptr->m_nPosition = 0;
    d_ptr->m_curPos = this->pos();
    d_ptr->p_shakeTimer->start();

    //TODO:net发送给聊天对象一个窗口抖动
}

void AbstractChatWidget::slot_ShakeTimeout()
{
    if(d_ptr->p_shakeTimer==NULL)
    {
        return ;
    }
    d_ptr->p_shakeTimer->stop();
    if(d_ptr->m_nPosition < MaxLimitTimes)
    {
        ++d_ptr->m_nPosition;
        switch(d_ptr->m_nPosition%4)
        {
        case 1:
        {
            QPoint tmpPos(d_ptr->m_curPos.x(),d_ptr->m_curPos.y()-MaxLimitSpace);
            this->move(tmpPos);
        }
            break;
        case 2:
        {
            QPoint tmpPos(d_ptr->m_curPos.x()-MaxLimitSpace,d_ptr->m_curPos.y()-MaxLimitSpace);
            this->move(tmpPos);
        }
            break;
        case 3:
        {
            QPoint tmpPos(d_ptr->m_curPos.x()-MaxLimitSpace,d_ptr->m_curPos.y());
            this->move(tmpPos);
        }
            break;
        default:
        case 0:
            this->move(d_ptr->m_curPos);
            break;
        }
        d_ptr->p_shakeTimer->start();
    }
}

//截图完成
void AbstractChatWidget::slot_ScreenShot_Ready(bool)
{
    if(!this->isActiveWindow())
    {
        return ;
    }
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if(mimeData->hasImage())
    {
        d_ptr->chatInputArea->insertCopyImage(clipboard->image());
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
    d_ptr->b_isScreeHide = flag;
}

void AbstractChatWidget::slot_ScreenTimeout()
{
    d_ptr->p_shotTimer->stop();
    if(d_ptr->p_shotProcess)
    {
        d_ptr->p_shotProcess->start( "rundll32 "+ QApplication::applicationDirPath()+ "/PrScrn.dll PrScrn" );
    }
}

//点击发送按钮，发送聊天编辑区域的信息
void AbstractChatWidget::slot_ButtClick_SendMsg(bool flag)
{
    Q_UNUSED(flag)

    if(d_ptr->chatInputArea->toPlainText().trimmed().isEmpty())
    {
        d_ptr->chatArea->insertTipChatText(QObject::tr("Input contents is empty."));
        return ;
    }
    TextUnit::ChatInfoUnit unit;
    d_ptr->chatInputArea->transTextToUnit(unit);
    QByteArray msg = d_ptr->chatInputArea->WriteJSONFile(unit);
    d_ptr->chatInputArea->clear();

    //TODO: 发送数据到网络

    //模拟测试已经接收到数据
    recvChatMsg(msg);
}

//判断是否Enter发送内容
void AbstractChatWidget::slot_CheckSendEnter()
{
    slot_ButtClick_SendMsg(true);
}

//响应数据库线程查询结果
void AbstractChatWidget::slot_DatabaseThread_ResultReady(int id, TextUnit::ChatInfoUnitList list)
{
    foreach(TextUnit::ChatInfoUnit unit,list)
    {
        d_ptr->chatArea->insertMeChatText(unit);
    }
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

void AbstractChatWidget::initChatRecord()
{
    d_ptr->p_DatabaseThread = new DatabaseThread(this);
    d_ptr->p_DatabaseThread->setDatabase(DatabaseManager::Instance()->getLastDB());

    connect(d_ptr->p_DatabaseThread,SIGNAL(resultReady(int,TextUnit::ChatInfoUnitList)),
            this,SLOT(slot_DatabaseThread_ResultReady(int,TextUnit::ChatInfoUnitList)),Qt::QueuedConnection);
    connect(d_ptr->p_DatabaseThread,SIGNAL(finished()),
            d_ptr->p_DatabaseThread,SLOT(deleteLater()));
    d_ptr->p_DatabaseThread->start();

    d_ptr->p_DatabaseThread->addSqlQueryTask(TestUserId,DatabaseManager::Instance()->querryRecords(TestUserId,5));
}

