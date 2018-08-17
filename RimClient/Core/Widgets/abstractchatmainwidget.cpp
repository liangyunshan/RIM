#include "abstractchatmainwidget.h"

#include <QDir>
#include <QMenu>
#include <QAction>
#include <QMimeData>
#include <QDateTime>
#include <QSplitter>
#include <QKeyEvent>
#include <QClipboard>
#include <QTabWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QJsonObject>
#include <QApplication>
#include <QHostAddress>
#include <QJsonDocument>
#include <QWebEngineView>
#include <QWebEnginePage>

#include "head.h"
#include "global.h"
#include "toolbar.h"
#include "document.h"
#include "constants.h"
#include "user/user.h"
#include "Util/rutil.h"
#include "screenshot.h"
#include "util/rsingleton.h"
#include "previewpage.h"
#include "messdiapatch.h"
#include "chataudioarea.h"
#include "widget/rlabel.h"
#include "widget/rbutton.h"
#include "media/audioinput.h"
#include "media/audiooutput.h"
#include "chat/setfontwidget.h"
#include "widget/rtoolbutton.h"
#include "widget/rmessagebox.h"
#include "thread/filerecvtask.h"
#include "chat/transferfileitem.h"
#include "thread/chatmsgprocess.h"
#include "others/msgqueuemanager.h"
#include "../protocol/datastruct.h"
#include "chat/transferfilelistbox.h"
#include "Network/msgwrap/wrapfactory.h"
#include "actionmanager/actionmanager.h"
#include "Widgets/textedit/simpletextedit.h"
#include "../thread/file716sendtask.h"
#include "../network/netglobal.h"
#include "../others/serialno.h"
#include "rquickorderwidget.h"
#include "Network/msgprocess/format495function.h"
#include "../file/globalconfigfile.h"

#define CHAT_MIN_WIDTH 450
#define CHAT_MIN_HEIGHT 500
#define CHAT_TOOL_HEIGHT 30
#define CHAT_USER_ICON_SIZE 30
#define TIMESTAMP_GAP 60            //显示新时间戳的消息时间间隔

//716-TK兼容
#include "../network/win32net/rudpsocket.h"
//

class AbstractChatMainWidgetPrivate : public GlobalData<AbstractChatMainWidget>
{
    Q_DECLARE_PUBLIC(AbstractChatMainWidget)

protected:
    explicit AbstractChatMainWidgetPrivate(AbstractChatMainWidget *q):
        q_ptr(q)
    {
        initWidget();
        fileList = NULL;
    }

    void initWidget();

    AbstractChatMainWidget * q_ptr;

    QWidget * chatRecordWidget;             //聊天信息记录窗口
    QWidget * inputWidget;                  //聊天信息输入窗口
    QTabWidget * rightSideWidget;           //右侧边栏窗口
    QWidget *rightWidget;

    QWebEngineView * view;                  //嵌入html页视图
    PreviewPage *page;                      //嵌入html页page
    Document m_content;                     //html与Qt交互跳板

    SetFontWidget *fontWidget;              //字体工具栏
    ChatAudioArea * chatAudioArea;          //录音工具栏
    RToolButton * shakeButt;                //窗口抖动按钮
    RToolButton * quickOrderButt;           //快捷信息发送按钮
    RToolButton * msgNoticeButt;            //消息提醒设置按钮
    RToolButton *recordButt;                //消息记录按钮
    ToolBar * chatToolBar;                  //信息输入窗口工具栏
    SimpleTextEdit * chatInputArea;         //信息输入窗口输入框
    QTabWidget *historyRecord;              //历史聊天记录窗口
    TransferFileListBox *fileList;          //文件传输列表

    SimpleUserInfo m_userInfo;              //当前聊天对象基本信息
    QString windowId;                       //窗口身份ID，只在创建时指定，可用于身份判断
    QDateTime m_preMsgTime;                 //上一条信息收发日期时间
    QDateTime m_nextMsgTime;                //下一条信息收发日期时间（在查看更多消息时使用）
    uint m_recordCount = 0;                 //当前显示的消息记录条数
#ifdef __LOCAL_CONTACT__
    ParameterSettings::OuterNetConfig netconfig;    /*!< 当前节点网络描述信息 */ 
#endif
};

void AbstractChatMainWidgetPrivate::initWidget()
{
    //主窗口水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(6,0,0,0);
    mainLayout->setSpacing(5);

    //左窗口垂直布局
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setSpacing(0);

    //聊天信息记录窗口
    chatRecordWidget = new QWidget(q_ptr);
    chatRecordWidget->setAttribute(Qt::WA_DeleteOnClose);
    page = new PreviewPage(chatRecordWidget);
    view = new QWebEngineView(chatRecordWidget);
    view->setWindowFlags(q_ptr->windowFlags() | Qt::FramelessWindowHint);   //FIXME LYS-20180718 修复显示大量数据时界面刷新问题
    view->setPage(page);
    QObject::connect(view,SIGNAL(loadFinished(bool)),q_ptr,SLOT(finishLoadHTML(bool)));

    fontWidget = new SetFontWidget(q_ptr);
    fontWidget->setVisible(false);
    QObject::connect(fontWidget,SIGNAL(fontModeChanged(int)),q_ptr,SLOT(setMsgShowMode(int)));
    QObject::connect(fontWidget,SIGNAL(fontChanged(QFont)),q_ptr,SLOT(setInputAreaFont(QFont)));
    QObject::connect(fontWidget,SIGNAL(fontColorChanged(QColor)),q_ptr,SLOT(setInputAreaColor(QColor)));

    chatAudioArea = new ChatAudioArea(q_ptr);
    chatAudioArea->setVisible(false);
    QObject::connect(chatAudioArea,SIGNAL(prepareSendAudio()),q_ptr,SLOT(prepareSendAudio()));
    QObject::connect(chatAudioArea,SIGNAL(preapreCancelAudio()),q_ptr,SLOT(preapreCancelAudio()));

    QVBoxLayout * chatLayout =  new QVBoxLayout;
    chatLayout->setContentsMargins(0,0,0,0);
    chatLayout->setSpacing(0);
    chatLayout->addWidget(view);
    chatLayout->addWidget(fontWidget);
    chatLayout->addWidget(chatAudioArea);
    chatRecordWidget->setLayout(chatLayout);

    //聊天信息输入窗口
    inputWidget = new QWidget(q_ptr);
    QVBoxLayout *inputLayout = new QVBoxLayout;
    inputLayout->setContentsMargins(0,0,0,0);
    inputLayout->setSpacing(0);
    inputWidget->setMinimumHeight(180);

    /**********聊天工具栏***************/
    chatToolBar = new ToolBar(inputWidget);
    chatToolBar->setObjectName("AbstractChatMainWidget_ChatToolBar");
    chatToolBar->setContentsMargins(5,0,5,0);
    chatToolBar->setFixedHeight(CHAT_TOOL_HEIGHT);
    chatToolBar->setSpacing(5);
    chatToolBar->setIconSize(QSize(CHAT_TOOL_HEIGHT,CHAT_TOOL_HEIGHT));

    //字体设置按钮
    RToolButton * fontButt = new RToolButton();
    fontButt->setObjectName(Constant::Tool_Chat_Font);
    fontButt->setToolTip(QObject::tr("Font"));
    fontButt->setCheckable(true);
    QObject::connect(fontButt,SIGNAL(clicked(bool)),q_ptr,SLOT(respFontArea(bool)));

    //表情按钮
    RToolButton * faceButt = new RToolButton();
    faceButt->setObjectName(Constant::Tool_Chat_Face);
    faceButt->setToolTip(QObject::tr("Emoji"));

    //窗口抖动按钮（仅单聊可用）
    shakeButt = new RToolButton();
    shakeButt->setObjectName(Constant::Tool_Chat_Shake);
    shakeButt->setToolTip(QObject::tr("Shake window"));
    QObject::connect(shakeButt,SIGNAL(clicked(bool)),q_ptr,SLOT(sendShakeWindow(bool)));

    //发送图片按钮
    RToolButton * imageButt = new RToolButton();
    imageButt->setObjectName(Constant::Tool_Chat_Image);
    imageButt->setToolTip(QObject::tr("Image"));

    //截图按钮
    RToolButton * screenShotButt = new RToolButton();
    screenShotButt->setObjectName(Constant::Tool_Chat_ScreenShot);
    screenShotButt->setToolTip(QObject::tr("Screenshot"));
    screenShotButt->setPopupMode(QToolButton::MenuButtonPopup);
    QObject::connect(screenShotButt,SIGNAL(clicked(bool)),G_pScreenShotAction,SIGNAL(triggered(bool)));

    //文件传输按钮
    RToolButton * fileTransButt = new RToolButton();
    fileTransButt->setObjectName(Constant::Tool_Chat_FileTrans);
    fileTransButt->setToolTip(QObject::tr("FileTrans"));
    QObject::connect(fileTransButt,SIGNAL(clicked(bool)),q_ptr,SLOT(sendTargetFiles(bool)));

    //快捷信息面板显示按钮
    quickOrderButt = new RToolButton();
    quickOrderButt->setObjectName(Constant::Tool_Chat_QuickOrder);
    quickOrderButt->setToolTip(QObject::tr("Show Quick Order panel"));
    QObject::connect(quickOrderButt,SIGNAL(clicked(bool)),q_ptr,SLOT(showQuickOrderWidget(bool)));

    QMenu * screenShotMenu = new QMenu(q_ptr);
    screenShotMenu->setObjectName(this->windowId + "ScreenShotMenu");
    screenShotMenu->addAction(G_pScreenShotAction);
    screenShotMenu->addAction(G_pHideWindowAction);
    screenShotButt->setMenu(screenShotMenu);

    //消息提醒设置按钮（仅群聊可用）
    msgNoticeButt = new RToolButton();
    msgNoticeButt->setObjectName(Constant::Tool_Chat_MsgNotice);
    msgNoticeButt->setToolTip(QObject::tr("NoticeSet"));

    //语音按钮
    RToolButton * audioButt = new RToolButton();
    audioButt->setObjectName(Constant::Tool_Chat_Audio);
    audioButt->setToolTip(QObject::tr("Audio"));
    QObject::connect(audioButt,SIGNAL(clicked(bool)),q_ptr,SLOT(respShowAudioArea(bool)));

    recordButt = new RToolButton();
    recordButt->setObjectName(Constant::Tool_Chat_Record);
    recordButt->setToolTip(QObject::tr("Record data"));
    recordButt->setCheckable(true);
    QObject::connect(recordButt,SIGNAL(toggled(bool)),q_ptr,SLOT(respHistoryRecord(bool)));

    chatToolBar->appendToolButton(faceButt);
    chatToolBar->appendToolButton(screenShotButt);
    chatToolBar->appendToolButton(imageButt);
    chatToolBar->appendToolButton(shakeButt);
    chatToolBar->appendToolButton(fontButt);
    chatToolBar->appendToolButton(msgNoticeButt);
    chatToolBar->appendToolButton(audioButt);
    chatToolBar->appendToolButton(fileTransButt);
    chatToolBar->appendToolButton(quickOrderButt);
    chatToolBar->addStretch(1);
    chatToolBar->appendToolButton(recordButt);

    QObject::connect(G_pScreenShot,SIGNAL(sig_ShotReady(bool)),q_ptr,SLOT(screenShotReady(bool)));

    chatInputArea = new SimpleTextEdit(inputWidget);
    chatInputArea->setFocus();
    QObject::connect(chatInputArea,SIGNAL(sigEnter()),q_ptr,SLOT(enterSend()));
    QObject::connect(chatInputArea,SIGNAL(sigDropFile(QString)),q_ptr,SLOT(dealDropFile(QString)));

    /**********底部按钮区***************/
    QWidget *buttonWidget = new QWidget(inputWidget);
    buttonWidget->setFixedHeight(35);
    QHBoxLayout * buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0,0,5,5);
    buttonLayout->setSpacing(5);

    RButton * closeButton = new RButton(buttonWidget);
    closeButton->setObjectName(Constant::Button_Chat_Close_Window);
    closeButton->setShortcut(ActionManager::instance()->shortcut(Constant::Button_Chat_Close_Window,QKeySequence("Alt+C")));
    closeButton->setText(QObject::tr("Close window"));
    QObject::connect(closeButton,SIGNAL(clicked()),q_ptr,SIGNAL(closeWindow()));

    RToolButton * extralButton = new RToolButton(buttonWidget);
    extralButton->setObjectName(Constant::Tool_Chat_SendMess);

    RButton * sendMessButton = new RButton(buttonWidget);
    sendMessButton->setObjectName(Constant::Button_Chat_Send);
    sendMessButton->setText(QObject::tr("Send message"));
    QObject::connect(sendMessButton,SIGNAL(clicked()),q_ptr,SLOT(enterSend()));

    QHBoxLayout * composeLayout = new QHBoxLayout;
    composeLayout->setSpacing(0);
    composeLayout->setContentsMargins(0,0,0,0);
    composeLayout->addWidget(sendMessButton);
    composeLayout->addWidget(extralButton);

    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addLayout(composeLayout);
    buttonWidget->setLayout(buttonLayout);

    inputLayout->addWidget(chatToolBar);
    inputLayout->addWidget(chatInputArea);
    inputLayout->addWidget(buttonWidget);
    inputWidget->setLayout(inputLayout);

    QSplitter *chatSplitter = new QSplitter(Qt::Vertical);
    chatSplitter->addWidget(chatRecordWidget);
    chatSplitter->addWidget(inputWidget);
    chatSplitter->setStretchFactor(0,5);
    chatSplitter->setStretchFactor(1,1);

    leftLayout->addWidget(chatSplitter);

    //聊天窗口右边栏（显示消息记录、发送文件子窗口）
    rightSideWidget = new QTabWidget(q_ptr);
    rightSideWidget->setMinimumWidth(300);
    rightSideWidget->setTabsClosable(true);
    rightSideWidget->setStyleSheet("QTabWidget::pane{"
                                        "border-top:1px solid #C4C4C3;"
                                        "border-left:0px;"
                                        "border-right:0px;"
                                        "border-bottom:0px;}"
                                   "QTabWidget::tab-bar {"
                                        "left: 0px; }"
                                   "QTabBar::tab {"
                                        "background-color: transparent;"
                                        "border-right:1px solid #C4C4C3;"
                                        "padding: 6px;}"
                                   "QTabBar::tab:selected{"
                                        "border-bottom:1px solid #C4C4C3;"
                                        "padding: 5px;}"
                                   "QTabBar::close-button{"
                                        "image:url(:/icon/resource/icon/Close-tab.png);}"
                                   "QTabBar::close-button:hover{"
                                        "image:url(:/icon/resource/icon/Close-tab-hover.png);}");
    QObject::connect(rightSideWidget,SIGNAL(tabCloseRequested(int)),q_ptr,SLOT(respCloseRightSideTab(int)));

    rightWidget = new QWidget(q_ptr);
    rightWidget->setObjectName("Chat_Right");
    rightWidget->setStyleSheet("QWidget#Chat_Right{"
                                    "border-left: 1px solid #C4C4C3;}");
    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setContentsMargins(0,0,0,0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(rightSideWidget);
    rightWidget->setLayout(rightLayout);
    rightWidget->setVisible(false);

    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(rightWidget);
    q_ptr->setLayout(mainLayout);
}

AbstractChatMainWidget::AbstractChatMainWidget(QWidget *parent) :
    d_ptr(new AbstractChatMainWidgetPrivate(this)),
    QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(CHAT_MIN_WIDTH,CHAT_MIN_HEIGHT);
    d_ptr->view->load(QUrl("qrc:/html/resource/html/chatRecord.html"));
    d_ptr->view->show();
    d_ptr->fontWidget->setDefault();
    this->setFocusPolicy(Qt::StrongFocus);
}

AbstractChatMainWidget::~AbstractChatMainWidget()
{
}

/*!
 * @brief 系统通知消息
 * @param type 系统通知消息的类型
 */
void AbstractChatMainWidget::onMessage(MessageType type)
{
    Q_UNUSED(type);
}

/*!
 * @brief 设置联系人基本信息
 * @param info 联系人基本信息
 */
void AbstractChatMainWidget::setUserInfo(const SimpleUserInfo &info)
{
    MQ_D(AbstractChatMainWidget);
    d->m_userInfo = info;
}

#ifdef __LOCAL_CONTACT__
/*!
 * @brief 设置节点外发信息配置
 * @param[in] config 节点外发配置信息
 * @note 该配置信息描述了与当前节点通信的网络描述信息，包括使用的通信方式，报文格式等。
 */
void AbstractChatMainWidget::setOuterNetConfig(const OuterNetConfig &config)
{
    MQ_D(AbstractChatMainWidget);
    d->netconfig = config;
}
#endif

/*!
 * @brief AbstractChatMainWidget::inserHtml 嵌入html文件
 * @param htmlUrl 待嵌入的html地址
 */
void AbstractChatMainWidget::inserHtml(QUrl &htmlUrl)
{
    MQ_D(AbstractChatMainWidget);
    d->view->load(htmlUrl);
    d->view->show();
}

/*!
 * @brief 设置窗口模式（单聊/群聊）
 * @param type 窗口模式（单聊/群聊）
 */
void AbstractChatMainWidget::setChatType(ChatType type)
{
    MQ_D(AbstractChatMainWidget);
    switch (type) {
    case C2C:
        d->shakeButt->setVisible(true);
        d->msgNoticeButt->setVisible(false);
        break;
    case GROUP:
        d->shakeButt->setVisible(false);
        d->msgNoticeButt->setVisible(true);
        break;
    default:
        break;
    }
}

/*!
 * @brief 播放语音消息
 * @param audioName 语音文件名称
 */
void AbstractChatMainWidget::playVoiceMessage(QString audioName)
{
    RSingleton<AudioOutput>::instance()->setAudioSaveDir(G_User->getC2CAudioPath());
    RSingleton<AudioOutput>::instance()->start(audioName);
}

/*!
 * @brief 当滚动条在最上方时继续向上滚动则查询显示更多消息记录
 */
void AbstractChatMainWidget::updateMsgRecord()
{
    MQ_D(AbstractChatMainWidget);

    RSingleton<ChatMsgProcess>::instance()->appendC2CMoreQueryTask(d->m_userInfo.accountId,d->m_recordCount,25);
}

/*!
 * @brief 查看更多消息模式下预览下一条消息日期时间
 * @param nextTime 下一条消息日期
 */
void AbstractChatMainWidget::setNextDateTime(QDateTime nextTime)
{
    MQ_D(AbstractChatMainWidget);

    d->m_nextMsgTime = nextTime;
}

void AbstractChatMainWidget::setChatChannel(QWebChannel *channel)
{
    MQ_D(AbstractChatMainWidget);

    d->page->setWebChannel(channel);
}

/*!
 * @brief 打开文件
 * @param filePath 目标文件全路径
 */
void AbstractChatMainWidget::openTargetFile(QString filePath)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
           <<""<<filePath
          <<"\n";
    QProcess::execute(QString("explorer %1").arg(QDir::toNativeSeparators(filePath)));
}

/*!
 * @brief 打开文件夹
 * @param folderPath 文件夹全路径
 */
void AbstractChatMainWidget::openTargetFolder(QString filePath)
{
    RUtil::showInExplorer(filePath);
}

void AbstractChatMainWidget::keyPressEvent(QKeyEvent *e)
{
    MQ_D(AbstractChatMainWidget);
    if(this->focusWidget() != d->chatInputArea)
    {
        d->chatInputArea->setFocus();
    }
    return QWidget::keyPressEvent(e);
}

/*!
 * @brief 加载完成html后对html进行初始化处理
 */
void AbstractChatMainWidget::finishLoadHTML(bool)
{
    setFontIconFilePath();
    emit initFinished();
}

/*!
 * @brief 窗口抖动
 * @param flag 抖动按钮标志
 */
void AbstractChatMainWidget::sendShakeWindow(bool flag)
{
    MQ_D(AbstractChatMainWidget);

    Q_UNUSED(flag);
    TextRequest * request = new TextRequest;
    request->msgCommand = MSG_TEXT_SHAKE;
    request->otherSideId = d->m_userInfo.accountId;
    request->accountId = G_User->BaseInfo().accountId;
    request->timeStamp = RUtil::timeStamp();
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);

    emit shakeWindow();
}

/*!
 * @brief 截图完成
 */
void AbstractChatMainWidget::screenShotReady(bool)
{
    MQ_D(AbstractChatMainWidget);
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

/*!
 * @brief 按下Enter键发送信息
 */
void AbstractChatMainWidget::enterSend()
{
    MQ_D(AbstractChatMainWidget);

    sendMsg();
    sendImg();
    d->chatInputArea->clear();
    d->chatInputArea->setFocus();
}

/*!
 * @brief 发送文字信息
 */
void AbstractChatMainWidget::sendMsg(bool flag)
{
    MQ_D(AbstractChatMainWidget);
    Q_UNUSED(flag);

    if(!d->chatInputArea->isVaiablePlaintext())
    {
        return ;
    }
#ifdef __LOCAL_CONTACT__
    sendMsg(d->chatInputArea->toPlainText());
#else
    QString t_simpleHtml = QString("");
    d->chatInputArea->extractPureHtml(t_simpleHtml);
    sendMsg(t_simpleHtml);
#endif
}

/*!
 * @brief 发送文字
 */
void AbstractChatMainWidget::sendMsg(QString str)
{
    MQ_D(AbstractChatMainWidget);

    //向历史会话记录列表插入一条记录
    HistoryChatRecord record;
    record.accountId = d->m_userInfo.accountId;
    record.nickName = d->m_userInfo.nickName;
    record.dtime = RUtil::currentMSecsSinceEpoch();
    record.lastRecord = RUtil::getTimeStamp();
    record.systemIon = d->m_userInfo.isSystemIcon;
    record.iconId = d->m_userInfo.iconId;
    record.type = CHAT_C2C;
    record.status = d->m_userInfo.status;
    MessDiapatch::instance()->onAddHistoryItem(record);

    ChatInfoUnit t_unit;
    t_unit.contentType = MSG_TEXT_TEXT;
    t_unit.accountId = G_User->BaseInfo().accountId;
    t_unit.nickName = G_User->BaseInfo().nickName;
    t_unit.dtime = RUtil::currentMSecsSinceEpoch();
    t_unit.dateTime = QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss");
    t_unit.msgstatus = ProtocolType::MSG_NOTREAD;
#ifdef __LOCAL_CONTACT__
    t_unit.serialNo = SERIALNO_FRASH;
    t_unit.contents = str;
#else
    t_unit.serialNo = record.dtime;
    QString t_sendHtml = str;
    RUtil::escapeSingleQuote(t_sendHtml);
    RUtil::escapeDoubleQuote(t_sendHtml);
    t_unit.contents = t_sendHtml;   //将转义处理后的内容存储到数据库中
#endif
    //存储发送信息到数据库
    RSingleton<ChatMsgProcess>::instance()->appendC2CStoreTask(d->m_userInfo.accountId,t_unit);
    appendMsgRecord(t_unit,SEND);

    //发送Html内容给联系人
    TextRequest * request = new TextRequest;
    request->type = OperatePerson;

    if(G_User->systemSettings()->encryptionCheck){
        //TODO 数据加密
    }
    request->isEncryption = G_User->systemSettings()->encryptionCheck;

    if(G_User->systemSettings()->compressCheck){
        //TODO 数据压缩
    }
    request->isCompress = G_User->systemSettings()->compressCheck;
    request->textType = TEXT_NORAML;
    request->accountId = G_User->BaseInfo().accountId;
    request->timeStamp = RUtil::timeStamp();
#ifdef __LOCAL_CONTACT__
    request->otherSideId = d->netconfig.nodeId;
    request->textId = QString::number(t_unit.serialNo);
    request->sendData = str;
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request,d->netconfig.communicationMethod,d->netconfig.messageFormat);

#else
    request->otherSideId = d->m_userInfo.accountId;
    request->textId = RUtil::UUID();
    request->sendData = t_sendHtml;     //FIXME LYS-20180608
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
#endif
}

/*!
 * @brief 发送信息出入框中的图片信息
 */
void AbstractChatMainWidget::sendImg()
{
    MQ_D(AbstractChatMainWidget);

    QStringList t_imgDirs;
    d->chatInputArea->getInputedImgs(t_imgDirs);
    if(!t_imgDirs.isEmpty())
    {
        for(int imgIndex=0;imgIndex<t_imgDirs.count();imgIndex++)
        {
            QString t_imgPath = t_imgDirs.at(imgIndex);
            dealDropFile(t_imgPath);
        }
        d->chatInputArea->clearInputImg();
    }
}

/*!
 * @brief 处理文字输入区域中拖放来的文件信息
 * @param fileName 文件绝对路径
 */
void AbstractChatMainWidget::dealDropFile(QString fileName)
{
    MQ_D(AbstractChatMainWidget);

    if(!Format495Function::checkFileCanbeSend(fileName))
    {
        QString note = tr("%1 size out of range").arg(QFileInfo(fileName).fileName());
        appendChatNotice(note,FAULT);
        return ;
    }

    TransferFileItem *item = appendTransferFile(fileName,TRANS_SEND);
    SenderFileDesc fileDesc;
    fileDesc.srcNodeId = G_User->BaseInfo().accountId;
    fileDesc.destNodeId = d->m_userInfo.accountId;
    fileDesc.fullFilePath = fileName;
    fileDesc.serialNo = item->taskSerialNo();

    SerialNo::instance()->updateSqlSerialNo(fileDesc.serialNo.toUShort());
    RSingleton<FileSendManager>::instance()->addFile(fileDesc);
}

/*!
 * @brief 控制历史聊天记录窗口
 * @param flag
 */
void AbstractChatMainWidget::setSideVisible(bool flag)
{
    MQ_D(AbstractChatMainWidget);

    d->rightWidget->setVisible(flag);
}

/*!
 * @brief 显示/隐藏字体设置区域
 * @param status 字体设置窗口状态（显示/隐藏）
 */
void AbstractChatMainWidget::respFontArea(bool status)
{
    Q_UNUSED(status);
    MQ_D(AbstractChatMainWidget);
    d->fontWidget->setVisible(status);
}

/*!
 * @brief 设置聊天信息记录窗口显示模式（气泡/文本）
 * @param mode
 */
void AbstractChatMainWidget::setMsgShowMode(int mode)
{
    if(mode == SetFontWidget::BUBBLEMODE)
    {
        //TODO LYS-20180606 切换至气泡模式
    }
    else
    {
        //TODO LYS-20180606 切换至文本模式
    }

}

/*!
 * @brief 设置输入框中字体
 * @param font 需要设置的字体
 */
void AbstractChatMainWidget::setInputAreaFont(const QFont &font)
{
    MQ_D(AbstractChatMainWidget);

    d->chatInputArea->setInputFont(font);
}

/*!
 * @brief 设置输入框中的字体颜色
 * @param color 需要设置的颜色
 */
void AbstractChatMainWidget::setInputAreaColor(const QColor &color)
{
    MQ_D(AbstractChatMainWidget);
    d->chatInputArea->setInputColor(color);
}

/*!
 * @brief 显示录音区域
 * @details 若其它窗口未在录音则直接录音；
 *          若其它窗口已经在录音，则提示，避免再打开录音功能
 */
void AbstractChatMainWidget::respShowAudioArea(bool)
{
    MQ_D(AbstractChatMainWidget);

    if(RSingleton<AudioInput>::instance()->isRecording())
    {
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
void AbstractChatMainWidget::prepareSendAudio()
{
    MQ_D(AbstractChatMainWidget);

    if(RSingleton<AudioInput>::instance()->stop())
    {
       QString lastRecordFileFullName = RSingleton<AudioInput>::instance()->lastRecordFullPath();

       //【聊天文件发送:1/5】向服务器发送文件
       if(G_User->isFileOnLine())
       {
           FileItemDesc * desc = new FileItemDesc;
           desc->id = RUtil::UUID();
           desc->fullPath = lastRecordFileFullName;
           desc->fileSize = QFileInfo(lastRecordFileFullName).size();
           desc->itemType = FILE_ITEM_CHAT_UP;
           desc->itemKind = FILE_AUDIO;
           desc->otherSideId = d->m_userInfo.accountId;

           FileRecvTask::instance()->addSendItem(desc);

           appendVoiceMsg(QFileInfo(lastRecordFileFullName).fileName(),SEND);
       }
    }
    d->chatAudioArea->setVisible(false);
}

/*!
 * @brief 取消录音文件发送
 * @details 1.将已经生成的文件删除
 *          2.隐藏录音区域
 */
void AbstractChatMainWidget::preapreCancelAudio()
{
    MQ_D(AbstractChatMainWidget);

    if(RSingleton<AudioInput>::instance()->stop()){

        RSingleton<AudioInput>::instance()->clear();
    }
    d->chatAudioArea->setVisible(false);
}

/*!
 * @brief 显示聊天记录查询结果
 */
void AbstractChatMainWidget::showQueryRecord(const ChatInfoUnit & msgUnit)
{
    MQ_D(AbstractChatMainWidget);

    if(msgUnit.accountId == d->m_userInfo.accountId)
    {
        appendMsgRecord(msgUnit,RECV);
    }
    else
    {
        appendMsgRecord(msgUnit,SEND);
    }
}

void AbstractChatMainWidget::showMoreQueryRecord(const ChatInfoUnit &msgUnit, bool hasNext)
{
    MQ_D(AbstractChatMainWidget);

    if(msgUnit.accountId == d->m_userInfo.accountId)
    {
        prependMsgRecord(msgUnit,RECV);
    }
    else
    {
        prependMsgRecord(msgUnit,SEND);
    }

    QDateTime t_curMsgTime = RUtil::addMSecsToEpoch(msgUnit.dtime);
    if(hasNext)
    {
        if(d->m_nextMsgTime.date() != t_curMsgTime.date())
        {
            prependChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(d->m_nextMsgTime.time().secsTo(t_curMsgTime.time())>= TIMESTAMP_GAP)
        {
            prependChatTimeNote(t_curMsgTime,TIME);
        }
    }
    else
    {
        prependChatTimeNote(t_curMsgTime,DATETIME);
    }
}

/*!
 * @brief 显示收到的消息
 * @param msg 收到的消息
 */
void AbstractChatMainWidget::recvTextChatMsg(const TextRequest &msg)
{
    ChatInfoUnit unit;
    unit.accountId = msg.accountId;
    unit.contents = msg.sendData;
    unit.dtime = msg.timeStamp;
    unit.contentType = msg.msgCommand;
    appendMsgRecord(unit,RECV);
}

/*!
 * @brief 聊天界面收到UDP信息
 * @details 显示接收到的UDP文字信息
 */
void AbstractChatMainWidget::slot_RecvRUDpData(QByteArray data)
{
    MQ_D(AbstractChatMainWidget);
    QString recvdata(data);

    QString t_showMsgScript = QString("appendMesRecord(%1,'%2')").arg(RECV).arg(recvdata);
    d->view->page()->runJavaScript(t_showMsgScript);
}

/*!
 * @brief 显示文件传输窗口
 * @details 选择需要发送的文件，支持多选
 */
void AbstractChatMainWidget::sendTargetFiles(bool)
{
    MQ_D(AbstractChatMainWidget);
    Q_UNUSED(d);
    QStringList files = QFileDialog::getOpenFileNames(
                              this,
                              tr("Select one or more files to open"),
                              tr("./"),
                              tr("All (*.*)"));
    if(files.isEmpty())
        return ;

    if(files.size() > Global::G_GlobalConfigFile->transSetting.maxTransFileOnce){
        RMessageBox::warning(this,tr("warning"),tr("Transfer files size greater than %1.").arg(Global::G_GlobalConfigFile->transSetting.maxTransFileOnce),RMessageBox::Yes);
        return;
    }

    foreach(QString fileName,files)
    {
        dealDropFile(fileName);
    }
}

/*!
 * @brief 显示快捷信息回复界面
 */
void AbstractChatMainWidget::showQuickOrderWidget(bool)
{
    MQ_D(AbstractChatMainWidget);

    QStringList list = RQuickOrderWidget::instance()->getCurrOrderList();
    QMenu *menu = new QMenu();
    QList<QAction*> actionList;
    foreach(QString order,list)
    {
        QAction* orderAction = new QAction(order);
        menu->addAction(orderAction);
        QObject::connect(orderAction,SIGNAL(triggered()),this,SLOT(sendQuickOrde()));
        actionList.append(orderAction);
    }
    menu->addSeparator();
    QAction* panelAction = new QAction(tr("Open panel"));
    panelAction->setIcon(QIcon(":/icon/resource/icon/icon_quickorder_panel.png"));
    menu->addAction(panelAction);
    QObject::connect(panelAction,SIGNAL(triggered()),this,SLOT(openQuickOrdePanel()));
    d->quickOrderButt->setMenu(menu);
    d->quickOrderButt->showMenu();
    delete menu;
    foreach(QAction *orderAction,actionList)
    {
        QObject::disconnect(orderAction,SIGNAL(triggered()),this,SLOT(sendQuickOrde()));
        delete orderAction;
        orderAction = NULL;
    }
    QObject::disconnect(panelAction,SIGNAL(triggered()),this,SLOT(openQuickOrdePanel()));
}

void AbstractChatMainWidget::sendQuickOrde()
{
    QAction *action = (QAction *)sender();
    if(action)
    {
        sendMsg(action->text());//单独发送文字
    }
}

/*!
 * @brief 打开快捷设置面板
 */
void AbstractChatMainWidget::openQuickOrdePanel()
{
    RQuickOrderWidget::instance()->raise();
    RQuickOrderWidget::instance()->showNormal();
}

/*!
 * @brief 更新文件传输tab内容
 */
void AbstractChatMainWidget::updateTransFileTab()
{
    MQ_D(AbstractChatMainWidget);

    int transFileIndex = d->rightSideWidget->indexOf(d->fileList);
    QString m_tabText = tr("Transfer Files")+"("+d->fileList->taskListProgress()+")";
    d->rightSideWidget->setTabText(transFileIndex,m_tabText);
    if(d->fileList->count() == 0)
    {
        closeRightSideTab(SendFile);
    }
}

/*!
 * @brief 响应文件传输交互单元中的取消操作
 */
void AbstractChatMainWidget::cancelTransfer(QString serialNo)
{
    MQ_D(AbstractChatMainWidget);

    foreach(TransferFileItem *item,d->fileList->items())
    {
        if(item->taskSerialNo() == serialNo)
        {
            SenderFileDesc fileDesc;
            fileDesc.srcNodeId = G_User->BaseInfo().accountId;
            fileDesc.destNodeId = d->m_userInfo.accountId;
            fileDesc.fullFilePath = item->fileName();
            fileDesc.serialNo = serialNo;

            RSingleton<FileSendManager>::instance()->deleteFile(fileDesc);
            break;
        }
    }
}

/*!
 * @brief 更新聊天界面的实时传输文件进度信息
 * @param progress 实时传输文件进度信息
 */
void AbstractChatMainWidget::updateTransFileStatus(FileTransProgress progress)
{
    MQ_D(AbstractChatMainWidget);

    if(progress.transStatus == TransStart && progress.transType == TRANS_RECV)
    {
        appendTransferFile(progress.fileFullPath,progress.transType);
    }
    if(d->fileList)
    {
        if(progress.transStatus == FileTransStatus::TransError)
        {
            QFileInfo info(progress.fileFullPath);
            QString note = tr("Trans File Error (%1): %2")
                    .arg(RUtil::formatFileSize(info.size()))
                    .arg(progress.fileFullPath);
            RUtil::StringToHtml(note);
            appendChatNotice(note,FAULT);
        }
        else if(progress.transStatus == FileTransStatus::TransCancel)
        {
            QFileInfo info(progress.fileFullPath);
            QString note = tr("Trans File Cancel (%1): %2")
                    .arg(RUtil::formatFileSize(info.size()))
                    .arg(progress.fileFullPath);
            RUtil::StringToHtml(note);
            appendChatNotice(note,NORMAL);
        }
        d->fileList->SetTransStatus(progress);
    }
}

/*!
 * @brief 更新界面中信息的已读未读状态
 */
void AbstractChatMainWidget::updateMsgStatus(ushort serialNo)
{
    MQ_D(AbstractChatMainWidget);

    //TODO LYS-20180718 更新界面中显示的读取状态
    QString stateID = QString::number(serialNo);
    QString t_setStateScript = QString("");
    t_setStateScript = QString("setMesReadState('%1')").arg(stateID);
    d->view->page()->runJavaScript(t_setStateScript);
}

/*!
 * @brief 显示收到的语音消息
 * @param msg 收到的语音消息
 */
void AbstractChatMainWidget::recvVoiceChatMsg(const QString &msg)
{
    appendVoiceMsg(msg);
}

/*!
 * @brief 响应右侧边栏窗口tab页关闭请求
 * @param index tab页索引值
 */
void AbstractChatMainWidget::respCloseRightSideTab(int index)
{
    MQ_D(AbstractChatMainWidget);

    d->rightSideWidget->removeTab(index);
    if(d->rightSideWidget->count() == 0)
    {
        setSideVisible(false);
        d->recordButt->setChecked(false);
    }
}

/*!
 * @brief 响应消息记录按钮操作
 * @param flag 操作标识
 */
void AbstractChatMainWidget::respHistoryRecord(bool flag)
{
    setSideVisible(flag);
    if(flag)
    {
        showRightSideTab(MsgRecord);
    }
    else
    {
        closeRightSideTab(MsgRecord);
    }
}

/*!
 * @brief 设置html中使用的字体图标文件所在路径
 */
void AbstractChatMainWidget::setFontIconFilePath()
{
    MQ_D(AbstractChatMainWidget);
    QString t_currentPath = QDir::currentPath();
    t_currentPath = QDir::fromNativeSeparators(t_currentPath);
    QString t_setLinkFontHerfScript = QString("setAbsoulteFontIconHerf('%1')").arg(t_currentPath);
    d->view->page()->runJavaScript(t_setLinkFontHerfScript);
}

/*!
 * @brief 根据需要关闭右边栏子窗口
 * @param tabType 子窗口类型
 */
void AbstractChatMainWidget::closeRightSideTab(RightTabType tabType)
{
    MQ_D(AbstractChatMainWidget);
    Q_UNUSED(d);
    switch (tabType) {
    case MsgRecord:
        //TODO LYS-20180620移除消息记录子窗口
        if(d->historyRecord)
        {
            d->rightSideWidget->removeTab(d->rightSideWidget->indexOf(d->historyRecord));
        }
        break;
    case SendFile:
        if(d->fileList)
        {
            d->rightSideWidget->removeTab(d->rightSideWidget->indexOf(d->fileList));
        }
        break;
    default:
        break;
    }

    if(d->rightSideWidget->count() == 0)
    {
        d->rightSideWidget->clear();
        setSideVisible(false);
    }

}

/*!
 * @brief 根据需要显示右边栏子窗口
 * @param tabType 子窗口类型
 */
void AbstractChatMainWidget::showRightSideTab(RightTabType tabType)
{
    MQ_D(AbstractChatMainWidget);

    d->rightWidget->setVisible(true);
    switch (tabType) {
    case MsgRecord:
        d->historyRecord = new QTabWidget;
        d->historyRecord->setStyleSheet("QTabWidget::pane{"
                                            "border:0px;}"
                                        "QTabWidget::tab-bar{"
                                            "left: 0px;}"
                                        "QTabBar::tab{"
                                            "border-radius: 2px;"
                                            "border:0px;"
                                            "width:50px;"
                                            "height:12px;"
                                            "margin-left:4px;"
                                            "margin-right:2px;"
                                            "margin-top:4px;}"
                                        "QTabBar::tab:selected{"
                                            "background-color: rgb(39,140,222);"
                                            "color:white;}"
                                        "QTabBar::tab:hover:!selected{"
                                            "background-color: rgb(39,140,222);"
                                            "color:black;}");
        d->historyRecord->setAttribute(Qt::WA_DeleteOnClose);
        d->historyRecord->addTab(new QWidget,tr("All"));
        d->historyRecord->addTab(new QWidget,tr("Image"));
        d->historyRecord->addTab(new QWidget,tr("File"));
        d->rightSideWidget->addTab(d->historyRecord,tr("message record"));
        d->rightSideWidget->setCurrentWidget(d->historyRecord);
        break;
    case SendFile:
    case RecvFile:
        if(!d->fileList)
        {
            d->fileList = new TransferFileListBox;
            d->fileList->setStyleSheet("QWidget{background-color:transparent;}");
            connect(d->fileList,SIGNAL(transferStatusChanged()),this,SLOT(updateTransFileTab()));
            connect(d->fileList,SIGNAL(cancelTransfer(QString)),this,SLOT(cancelTransfer(QString)));
        }
        d->rightSideWidget->addTab(d->fileList,tr("Transfer Files"));
        d->rightSideWidget->setCurrentWidget(d->fileList);

        break;
    default:
        break;
    }
}

/*!
 * @brief 界面显示传送文件队列
 * @param fileName 待传送的文件名
 */
TransferFileItem* AbstractChatMainWidget::appendTransferFile(QString &fileName,TransType transType)
{
    MQ_D(AbstractChatMainWidget);

    RightTabType type;
    TransferFileItem::TransferType transferType;
    if(transType == TRANS_RECV)
    {
        type = RecvFile;
        transferType = TransferFileItem::RECVFile;
    }
    else
    {
        type = SendFile;
        transferType = TransferFileItem::SENDFile;
    }
    showRightSideTab(type);

    QFileInfo fileInfo(fileName);
    TransferFileItem *t_item = new TransferFileItem;
    t_item->setFileType(TransferFileItem::COMMONFILE);
    t_item->setTransferType(transferType);
    t_item->setFileName(fileName);
    t_item->setFileSize(fileInfo.size());
    t_item->setTaskSerialNo(QString::number(SERIALNO_FRASH));
    t_item->setFinishedSize(0);
    d->fileList->addItem(t_item);

    updateTransFileTab();
    return t_item;

}

/*!
 * @brief 将收到的信息追加显示在聊天信息记录界面上
 * @param recvMsg 收到的信息
 * @param source 信息来源（接收）
 */
void AbstractChatMainWidget::appendMsgRecord(const TextRequest &recvMsg, MsgTarget source)
{
    MQ_D(AbstractChatMainWidget);
    Q_UNUSED(source);
    d->m_recordCount++;
    QString t_showMsgScript = QString("");
    QDateTime t_epochTime(QDate(1970,1,1),QTime(8,0,0));
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

//    RUtil::removeEccapeDoubleQuote(t_localHtml);
    RUtil::setAbsoulteImgPath(t_localHtml,G_User->BaseInfo().accountId);

    t_headPath = G_User->getIconAbsoultePath(d->m_userInfo.isSystemIcon,d->m_userInfo.iconId);
    t_showMsgScript = QString("appendMesRecord(%1,'%2','%3',%4,'%5')").arg(RECV).arg(t_localHtml).arg(t_headPath)
                                                            .arg(UNREAD).arg(recvMsg.textId);

    d->view->page()->runJavaScript(t_showMsgScript);
}

/*!
 * @brief 将收/发信息追加显示在聊天信息记录界面上
 * @param unitMsg 收发的信息
 * @param source 信息来源（接收/发送）
 */
void AbstractChatMainWidget::appendMsgRecord(const ChatInfoUnit &unitMsg, MsgTarget source)
{
    MQ_D(AbstractChatMainWidget);
    d->m_recordCount++;
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

//    RUtil::setAbsoulteImgPath(t_localHtml,G_User->BaseInfo().accountId);
    RUtil::StringToHtml(t_localHtml);
#ifdef __LOCAL_CONTACT__
    t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
#else
    if(source == RECV)
    {
        t_headPath = G_User->getIconAbsoultePath(d->m_userInfo.isSystemIcon,d->m_userInfo.iconId);
    }
    else
    {
        t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
    }
#endif

    if(unitMsg.contentType == MSG_TEXT_TEXT)
    {
        QString stateID = QString::number(unitMsg.serialNo);
        int t_readState = (unitMsg.msgstatus == ProtocolType::MSG_NOTREAD) ? UNREAD : MARKREAD;
        t_showMsgScript = QString("appendMesRecord(%1,'%2','%3',%4,'%5')").arg(source).arg(t_localHtml).arg(t_headPath)
                                                                     .arg(t_readState).arg(stateID);
    }
    else if(unitMsg.contentType == MSG_TEXT_FILE)
    {
        QString t_filePath = unitMsg.contents;
        t_showMsgScript = QString("appendFile(%1,'%2','%3',%4)").arg(source).arg(t_filePath).arg(t_headPath)
                                                                     .arg(1);
    }

    d->view->page()->runJavaScript(t_showMsgScript);
}

/*!
 * @brief 将收到的信息前置显示在聊天信息记录界面上
 * @param recvMsg 收到的信息
 * @param source 信息来源（接收）
 */
void AbstractChatMainWidget::prependMsgRecord(const TextRequest &recvMsg, MsgTarget source)
{
    MQ_D(AbstractChatMainWidget);

    Q_UNUSED(source);
    d->m_recordCount++;
    QString t_showMsgScript = QString("");
    QDateTime t_epochTime(QDate(1970,1,1),QTime(8,0,0));
    QDateTime t_curMsgTime = t_epochTime.addMSecs(recvMsg.timeStamp);
    QString t_localHtml = recvMsg.sendData;
    QString t_headPath = QString("");

//    RUtil::removeEccapeDoubleQuote(t_localHtml);
    RUtil::setAbsoulteImgPath(t_localHtml,G_User->BaseInfo().accountId);

    t_headPath = G_User->getIconAbsoultePath(d->m_userInfo.isSystemIcon,d->m_userInfo.iconId);

    t_showMsgScript = QString("prependMesRecord(%1,'%2','%3',%4,'%5')").arg(RECV).arg(t_localHtml).arg(t_headPath)
                                                                    .arg(UNREAD).arg(recvMsg.textId);

    d->view->page()->runJavaScript(t_showMsgScript);

    if(d->m_preMsgTime.isNull())
    {
        d->m_preMsgTime = t_curMsgTime;
        if(d->m_preMsgTime.date() != G_loginTime.date())
        {
            prependChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(G_loginTime.time().secsTo(d->m_preMsgTime.time())>= TIMESTAMP_GAP)
        {
            prependChatTimeNote(t_curMsgTime,TIME);
        }
    }
    else
    {
        if(t_curMsgTime.date() != d->m_preMsgTime.date())
        {
            prependChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(d->m_preMsgTime.time().secsTo(t_curMsgTime.time()) >= TIMESTAMP_GAP)
        {
            prependChatTimeNote(t_curMsgTime,TIME);
        }
        d->m_preMsgTime = t_curMsgTime;
    }
}

/*!
 * @brief 将收/发信息前置显示在聊天信息记录界面上
 * @param unitMsg 收发的信息
 * @param source 信息来源（接收/发送）
 */
void AbstractChatMainWidget::prependMsgRecord(const ChatInfoUnit &unitMsg, MsgTarget source)
{
    MQ_D(AbstractChatMainWidget);

    d->m_recordCount++;
    QString t_showMsgScript = QString("");
    QString t_localHtml = unitMsg.contents;
    QDateTime t_curMsgTime = RUtil::addMSecsToEpoch(unitMsg.dtime);
    QString t_headPath = QString("");

//    RUtil::setAbsoulteImgPath(t_localHtml,G_User->BaseInfo().accountId);
    RUtil::escapeSingleQuote(t_localHtml);

    if(source == RECV)
    {
        t_headPath = G_User->getIconAbsoultePath(d->m_userInfo.isSystemIcon,d->m_userInfo.iconId);
    }
    else
    {
        t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
    }

    if(unitMsg.contentType == MSG_TEXT_TEXT)
    {
        QString stateID = QString::number(unitMsg.serialNo);
        int t_readState = (unitMsg.msgstatus == ProtocolType::MSG_NOTREAD) ? UNREAD : MARKREAD;
        t_showMsgScript = QString("prependMesRecord(%1,'%2','%3',%4,'%5')").arg(source).arg(t_localHtml).arg(t_headPath)
                                                                        .arg(t_readState).arg(stateID);
    }
    else if(unitMsg.contentType == MSG_TEXT_FILE)
    {
        QString t_filePath = unitMsg.contents;
        t_showMsgScript = QString("prependFile(%1,'%2','%3',%4)").arg(source).arg(t_filePath).arg(t_headPath)
                                                                     .arg(1);
    }

    d->view->page()->runJavaScript(t_showMsgScript);

    if(d->m_preMsgTime.isNull())
    {
        d->m_preMsgTime = t_curMsgTime;
        if(d->m_preMsgTime.date() != G_loginTime.date())
        {
            prependChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(G_loginTime.time().secsTo(d->m_preMsgTime.time())>= TIMESTAMP_GAP)
        {
            prependChatTimeNote(t_curMsgTime,TIME);
        }
    }
    else
    {
        if(t_curMsgTime.date() != d->m_preMsgTime.date())
        {
            prependChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(d->m_preMsgTime.time().secsTo(t_curMsgTime.time()) >= TIMESTAMP_GAP)
        {
            prependChatTimeNote(t_curMsgTime,TIME);
        }
        d->m_preMsgTime = t_curMsgTime;
    }
}

/*!
 * @brief 将收发的语音信息追加显示在聊天信息记录界面上
 * @param recordFileName 语音文件路径
 * @param source 信息来源（接收/发送）
 */
void AbstractChatMainWidget::appendVoiceMsg(QString recordFileName, MsgTarget source)
{
    MQ_D(AbstractChatMainWidget);
    QString t_headPath;
    if(source == SEND)
    {
        t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
    }
    else if(source == RECV)
    {
        t_headPath = G_User->getIconAbsoultePath(d->m_userInfo.isSystemIcon,d->m_userInfo.iconId);
    }

    QString t_showVoiceMsgScript = QString("appendVoiceMsg(%1,'%2','%3','%4')").arg(source).arg(50).arg(recordFileName).arg(t_headPath);
    d->view->page()->runJavaScript(t_showVoiceMsgScript);
}

/*!
 * @brief 将收发的语音信息前置显示在聊天信息记录界面上
 * @param recordFileName 语音文件路径
 * @param source 信息来源（接收/发送）
 */
void AbstractChatMainWidget::prependVoiceMsg(QString recordFileName, AbstractChatMainWidget::MsgTarget source)
{
    MQ_D(AbstractChatMainWidget);

    QString t_headPath;
    if(source == SEND)
    {
        t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
    }
    else if(source == RECV)
    {
        t_headPath = G_User->getIconAbsoultePath(d->m_userInfo.isSystemIcon,d->m_userInfo.iconId);
    }

    QString t_showVoiceMsgScript = QString("prependVoiceMsg(%1,'%2','%3','%4')").arg(source).arg(50).arg(recordFileName).arg(t_headPath);
    d->view->page()->runJavaScript(t_showVoiceMsgScript);
}

/*!
 * @brief 将收发的图片信息追加显示在聊天信息记录界面上
 * @param unitMsg 图片消息
 * @param source 信息来源（接收/发送）
 */
void AbstractChatMainWidget::appendImageMsg(const ChatInfoUnit &unitMsg, MsgTarget source)
{
    MQ_D(AbstractChatMainWidget);
    d->m_recordCount++;
    QString t_showMsgScript = QString("");
    QString t_imgPath = unitMsg.contents;
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

    if(source == RECV)
    {
        t_headPath = G_User->getIconAbsoultePath(d->m_userInfo.isSystemIcon,d->m_userInfo.iconId);
    }
    else
    {
        t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
    }

    if(unitMsg.contentType == MSG_TEXT_IMAGE)
    {
        t_showMsgScript = QString("appendImgRecord(%1,'%2','%3')").arg(source).arg(t_imgPath).arg(t_headPath);
    }

    d->view->page()->runJavaScript(t_showMsgScript);
}

/*!
 * @brief 将收发的图片信息前置显示在聊天信息记录界面上
 * @param unitMsg 图片消息
 * @param source 信息来源（接收/发送）
 */
void AbstractChatMainWidget::prependImageMsg(const ChatInfoUnit &unitMsg, MsgTarget source)
{
    MQ_D(AbstractChatMainWidget);

    d->m_recordCount++;
    QString t_showMsgScript = QString("");
    QString t_imgPath = unitMsg.contents;
    QDateTime t_curMsgTime = RUtil::addMSecsToEpoch(unitMsg.dtime);
    QString t_headPath = QString("");

    if(source == RECV)
    {
        t_headPath = G_User->getIconAbsoultePath(d->m_userInfo.isSystemIcon,d->m_userInfo.iconId);
    }
    else
    {
        t_headPath = G_User->getIconAbsoultePath(G_User->BaseInfo().isSystemIcon,G_User->BaseInfo().iconId);
    }

    if(unitMsg.contentType == MSG_TEXT_IMAGE)
    {
        t_showMsgScript = QString("prependImgRecord(%1,'%2','%3')").arg(source).arg(t_imgPath).arg(t_headPath);
    }

    d->view->page()->runJavaScript(t_showMsgScript);

    if(d->m_preMsgTime.isNull())
    {
        d->m_preMsgTime = t_curMsgTime;
        if(d->m_preMsgTime.date() != G_loginTime.date())
        {
            prependChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(G_loginTime.time().secsTo(d->m_preMsgTime.time())>= TIMESTAMP_GAP)
        {
            prependChatTimeNote(t_curMsgTime,TIME);
        }
    }
    else
    {
        if(t_curMsgTime.date() != d->m_preMsgTime.date())
        {
            prependChatTimeNote(t_curMsgTime,DATETIME);
        }
        else if(d->m_preMsgTime.time().secsTo(t_curMsgTime.time()) >= TIMESTAMP_GAP)
        {
            prependChatTimeNote(t_curMsgTime,TIME);
        }
        d->m_preMsgTime = t_curMsgTime;
    }
}

/*!
 * @brief 显示操作提示信息
 * @param content 信息内容
 * @param type 信息类型
 */
void AbstractChatMainWidget::appendChatNotice(QString content, NoticeType type)
{
    MQ_D(AbstractChatMainWidget);
    if(content.isEmpty())
    {
        return;
    }
    QString t_showNotice = QString("appendNotice(%1,'%2')").arg(type).arg(content);
    d->view->page()->runJavaScript(t_showNotice);
}

/*!
 * @brief 前置显示操作提示信息
 * @param content 信息内容
 * @param type 信息类型
 */
void AbstractChatMainWidget::prependChatNotice(QString content, AbstractChatMainWidget::NoticeType type)
{
    MQ_D(AbstractChatMainWidget);

    if(content.isEmpty())
    {
        return;
    }
    QString t_showNotice = QString("prependNotice(%1,'%2')").arg(type).arg(content);
    d->view->page()->runJavaScript(t_showNotice);
}

/*!
 * @brief 显示时间提示信息
 * @param content 时间显示格式
 * @param format 显示内容
 */
void AbstractChatMainWidget::appendChatTimeNote(QDateTime content, AbstractChatMainWidget::TimeFormat format)
{
    MQ_D(AbstractChatMainWidget);
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
    t_showTimeScript = QString("appendMessageTime('%1')").arg(t_curMsgTime);
    d->view->page()->runJavaScript(t_showTimeScript);
}

/*!
 * @brief 前置显示时间提示信息
 * @param content 时间显示格式
 * @param format 显示内容
 */
void AbstractChatMainWidget::prependChatTimeNote(QDateTime content, AbstractChatMainWidget::TimeFormat format)
{
    MQ_D(AbstractChatMainWidget);

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
    t_showTimeScript = QString("prependMessageTime('%1')").arg(t_curMsgTime);
    d->view->page()->runJavaScript(t_showTimeScript);
}
