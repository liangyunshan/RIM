#include "abstractchatwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QApplication>
#include <QDesktopWidget>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "toolbar.h"
#include "Util/rutil.h"
#include "widget/riconlabel.h"
#include "util/rsingleton.h"
#include "util/imagemanager.h"

#include "actionmanager/actionmanager.h"
#include "toolbar.h"

#define CHAT_MIN_WIDTH 450
#define CHAT_MIN_HEIGHT 500

class AbstractChatWidgetPrivate : public GlobalData<AbstractChatWidget>
{
    Q_DECLARE_PUBLIC(AbstractChatWidget)

protected:
    AbstractChatWidgetPrivate(AbstractChatWidget * q):
        q_ptr(q)
    {
        initWidget();
        windowId = RUtil::UUID();
    }

    void initWidget();

    AbstractChatWidget * q_ptr;

    QWidget * contentWidget;

    QWidget * userInfoWidget;       //用户、群组头像等信息页面
    RIconLabel * userInfo_IconLabel;
    QLabel * userInfo_NameLabel;

    ToolBar * toolBar;              //工具栏

    QSplitter * splitter;
    QSplitter * chatSplitter;
    QWidget * bodyWidget;           //正文区域
    QWidget * leftWidget;
    QWidget * rightWidget;

    QWidget * chatArea;             //聊天区域
    ToolBar * chatToolBar;          //聊天工具栏
    QWidget * chatInputArea;        //信息输入框
    QWidget * buttonWidget;         //关闭、发送等按钮栏

    ToolBar * windowToolBar;

    QString windowId;
};

#define CHAT_TOOL_HEIGHT 30
#define CHAT_USER_ICON_SIZE 30

void AbstractChatWidgetPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("ContentWidget");
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
    userInfo_IconLabel->setPixmap(QPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon()));
    userInfo_IconLabel->setFixedSize(CHAT_USER_ICON_SIZE,CHAT_USER_ICON_SIZE);

    userInfo_NameLabel = new QLabel();
    userInfo_NameLabel->setObjectName("Chat_User_NameLabel");
    userInfo_NameLabel->setText("韩天才");
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

//    userInfoWidget->setStyleSheet("border-bottom:1px solid red");

    /**********工具栏***************/
    toolBar = new ToolBar(contentWidget);
    toolBar->setFixedHeight(CHAT_TOOL_HEIGHT);

    RToolButton * callButton = ActionManager::instance()->createToolButton(Constant::Tool_Chat_Call);
    callButton->setFixedSize(28,24);

    toolBar->appendToolButton(callButton);
    toolBar->addStretch(1);

    QHBoxLayout * bodyLayout = new QHBoxLayout();
    bodyLayout->setContentsMargins(0,0,0,0);
    bodyLayout->setSpacing(0);

    bodyWidget = new QWidget(contentWidget);
    splitter = new QSplitter(bodyWidget);

    leftWidget = new QWidget(contentWidget);
    rightWidget = new QWidget(contentWidget);

    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setSpacing(0);

    chatArea = new QWidget(leftWidget);

    QWidget * chatInputContainter = new QWidget(leftWidget);
    chatInputContainter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    chatInputContainter->setMinimumHeight(110);
    QVBoxLayout *chatLayout = new QVBoxLayout;
    chatLayout->setContentsMargins(0,0,0,0);
    chatLayout->setSpacing(0);

    chatToolBar = new ToolBar(chatInputContainter);
    chatToolBar->setFixedHeight(CHAT_TOOL_HEIGHT);

    RToolButton * fontAction = ActionManager::instance()->createToolButton(Constant::Tool_chat_Font);
    fontAction->setToolTip(QObject::tr("Font"));

    RToolButton * faceAction = ActionManager::instance()->createToolButton(Constant::Tool_chat_Face);
    faceAction->setToolTip(QObject::tr("Emoji"));

    RToolButton * shakeAction = ActionManager::instance()->createToolButton(Constant::Tool_chat_Shake);
    shakeAction->setToolTip(QObject::tr("Shake window"));

    RToolButton * imageAction = ActionManager::instance()->createToolButton(Constant::Tool_chat_Image);
    imageAction->setToolTip(QObject::tr("Image"));

    RToolButton * screenShotAction = ActionManager::instance()->createToolButton(Constant::Tool_chat_ScreenShot);
    screenShotAction->setToolTip(QObject::tr("Screen shot"));

    RToolButton * recordAction = ActionManager::instance()->createToolButton(Constant::Tool_chat_Record);
    recordAction->setToolTip(QObject::tr("Chat"));

    chatToolBar->appendToolButton(fontAction);
    chatToolBar->appendToolButton(faceAction);
    chatToolBar->appendToolButton(shakeAction);
    chatToolBar->appendToolButton(imageAction);
    chatToolBar->appendToolButton(screenShotAction);
    chatToolBar->addStretch(1);
    chatToolBar->appendToolButton(recordAction);

    chatInputArea = new QWidget(chatInputContainter);

    buttonWidget = new QWidget(chatInputContainter);
    buttonWidget->setFixedHeight(CHAT_TOOL_HEIGHT);

    chatLayout->addWidget(chatToolBar);
    chatLayout->addWidget(chatInputArea);
    chatLayout->addWidget(buttonWidget);
    chatInputContainter->setLayout(chatLayout);

    chatSplitter = new QSplitter(Qt::Vertical);
    chatSplitter->addWidget(chatArea);
    chatSplitter->addWidget(chatInputContainter);
    chatSplitter->setStretchFactor(0,3);
    chatSplitter->setStretchFactor(1,1);

    leftLayout->addWidget(chatSplitter);

    leftWidget->setLayout(leftLayout);

    QVBoxLayout * rightLayout = new QVBoxLayout;
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->setSpacing(0);
    rightWidget->setLayout(rightLayout);

    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);

    splitter->setStretchFactor(0,3);
    splitter->setStretchFactor(1,1);

    bodyLayout->addWidget(splitter);
    bodyWidget->setLayout(bodyLayout);

    contentLayout->addWidget(userInfoWidget);
    contentLayout->addWidget(toolBar);
    contentLayout->addWidget(bodyWidget);

    contentWidget->setLayout(contentLayout);
    q_ptr->setContentWidget(contentWidget);

    /**********窗口控制区***************/
    windowToolBar = new ToolBar(contentWidget);
    windowToolBar->setContentsMargins(5,0,0,0);

    RToolButton * minSize = ActionManager::instance()->createToolButton(Id(Constant::Tool_Chat_Min),q_ptr,SLOT(showMinimized()));
    minSize->setIcon(QIcon(QString(":/icon/resource/icon/%1.png").arg(Constant::TOOL_MIN)));
    minSize->setToolTip(QObject::tr("Min"));

    RToolButton * closeButt = ActionManager::instance()->createToolButton(Id(Constant::Tool_Chat_Close),q_ptr,SLOT(closeWindow()));
    closeButt->setIcon(QIcon(QString(":/icon/resource/icon/%1.png").arg(Constant::TOOL_CLOSE)));
    closeButt->setToolTip(QObject::tr("Close"));

    windowToolBar->addStretch(1);
    windowToolBar->appendToolButton(minSize);
    windowToolBar->appendToolButton(closeButt);
}

#define WINDOW_SIZ_WIDTH_FACTOR 0.4
#define WINDOW_SIZ_HEIGHT_FACTOR 0.80

AbstractChatWidget::AbstractChatWidget(QWidget *parent):
    d_ptr(new AbstractChatWidgetPrivate(this)),
    AbstractWidget(parent)
{
    setMinimumSize(CHAT_MIN_WIDTH,CHAT_MIN_HEIGHT);

    QSize size = QApplication::desktop()->size();
    resize(size.width() * WINDOW_SIZ_WIDTH_FACTOR,size.height() * WINDOW_SIZ_HEIGHT_FACTOR);

    setWindowFlags(windowFlags() &(~Qt::Tool));
}

QString AbstractChatWidget::widgetId()
{
    MQ_D(AbstractChatWidget);
    return d->windowId;
}

void AbstractChatWidget::resizeEvent(QResizeEvent *)
{
    MQ_D(AbstractChatWidget);
    d->windowToolBar->setGeometry(WINDOW_MARGIN_SIZE,0,width() - 2 * WINDOW_MARGIN_SIZE,Constant::TOOL_WIDTH);
}


