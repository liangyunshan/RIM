#include "historymsgrecord.h"

#include <QDir>
#include <QLabel>
#include <QPoint>
#include <QProcess>
#include <QComboBox>
#include <QLineEdit>
#include <QDateTime>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QStackedWidget>
#include <QCalendarWidget>

#include "../toolbar.h"
#include "../../head.h"
#include "../../global.h"
#include "historybridge.h"
#include "../Util/rutil.h"
#include "../previewpage.h"
#include "../../constants.h"
#include "../../user/user.h"
#include "../widget/rtoolbutton.h"
#include "../../../Util/rsingleton.h"
#include "../../protocol/datastruct.h"
#include "../../thread/chatmsgprocess.h"

#define Filiter_TOOL_HEIGHT 30
#define HISTORY_MIN_WIDTH 50
#define HISTORY_MIN_HEIGHT 450

class HistoryMsgRecordPrivate : public GlobalData<HistoryMsgRecord>
{
    Q_DECLARE_PUBLIC(HistoryMsgRecord)

protected:
    explicit HistoryMsgRecordPrivate(HistoryMsgRecord *q):
        q_ptr(q)
    {
        initWidget();
    }

    void initWidget();

    HistoryMsgRecord * q_ptr;

    QTabWidget * mainWidget;
    HistoryBridge *m_bridge;
    QWebChannel *m_channel;
    QWebEngineView * allView;           //全部历史消息显示视图
    QWebEngineView * imgView;           //图片历史消息显示视图
    QWebEngineView * fileView;          //文件历史消息显示视图
    QWidget *searchWidget;              //搜索条窗口
    RToolButton * dateButt;             //日期选择按钮
    QCalendarWidget *chooseDate;        //日期选择窗口

    SimpleUserInfo m_userInfo;          //当前聊天对象基本信息

};

void HistoryMsgRecordPrivate::initWidget()
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(1,0,0,0);
    mainLayout->setSpacing(0);

    mainWidget = new QTabWidget;
    mainWidget->setStyleSheet("QTabWidget::pane{"
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
    mainWidget->setAttribute(Qt::WA_DeleteOnClose);

    m_channel = new QWebChannel(mainWidget);
    m_bridge = new HistoryBridge;
    m_channel->registerObject(QStringLiteral("hisBridge"),m_bridge);

    //全部类型历史消息
    QWidget * allHisRecord = new QWidget(mainWidget);
    allHisRecord->setAttribute(Qt::WA_DeleteOnClose);
    PreviewPage * allPage = new PreviewPage(allHisRecord);
    allView = new QWebEngineView(allHisRecord);
    allView->setObjectName(Constant::HISTORY_WEBVIEW_ALL);
    allView->setWindowFlags(allHisRecord->windowFlags() | Qt::FramelessWindowHint);
    allView->setPage(allPage);
    QObject::connect(allView,SIGNAL(loadFinished(bool)),q_ptr,SLOT(finishLoadHTML(bool)));
    allPage->setWebChannel(m_channel);
    //全部历史消息筛选工具栏
    ToolBar * filiterToolBar = new ToolBar;
    filiterToolBar->setContentsMargins(2,2,2,2);
    filiterToolBar->setFixedHeight(Filiter_TOOL_HEIGHT);

    //历史记录设置按钮
    RToolButton * setButt = new RToolButton();
    setButt->setIconSize(QSize(Constant::TOOL_WIDTH-13,Constant::TOOL_HEIGHT-13));
    setButt->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setButt->setObjectName(Constant::Tool_History_Set);
    setButt->setToolTip(QObject::tr("Set"));
    setButt->setText(QObject::tr("Set"));

    //打开搜索条按钮
    RToolButton * openSearchButt = new RToolButton();
    openSearchButt->setIconSize(QSize(Constant::TOOL_WIDTH-10,Constant::TOOL_HEIGHT-10));
    openSearchButt->setObjectName(Constant::Tool_History_Search);
    openSearchButt->setToolTip(QObject::tr("Open Search Item"));
    openSearchButt->setCheckable(true);
    QObject::connect(openSearchButt,SIGNAL(clicked(bool)),q_ptr,SLOT(respOpenSearch(bool)));

    //消息管理器按钮
    RToolButton * manageButt = new RToolButton();
    manageButt->setIconSize(QSize(Constant::TOOL_WIDTH-10,Constant::TOOL_HEIGHT-10));
    manageButt->setObjectName(Constant::Tool_History_Manage);
    manageButt->setToolTip(QObject::tr("Message Manager"));

    //日期选择按钮
    dateButt = new RToolButton();
    dateButt->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    dateButt->setObjectName(Constant::Tool_History_Calendar);
    dateButt->setText("2018-08-10");

    //日期选择窗口
    chooseDate = new QCalendarWidget();
    chooseDate->setWindowFlags(Qt::Popup);
    chooseDate->setVisible(false);
    QObject::connect(dateButt,SIGNAL(clicked()),q_ptr,SLOT(respChooseDate()));
    QObject::connect(chooseDate,SIGNAL(clicked(const QDate &)),q_ptr,SLOT(respSetDate(const QDate &)));

    //第一页按钮
    RToolButton * firstButt = new RToolButton();
    firstButt->setObjectName(Constant::Tool_History_First);
    firstButt->setToolTip(QObject::tr("First Page"));
    QObject::connect(firstButt,SIGNAL(clicked()),q_ptr,SLOT(respSwitchPage()));
    //上一页按钮
    RToolButton * prevButt = new RToolButton();
    prevButt->setObjectName(Constant::Tool_History_Previous);
    prevButt->setToolTip(QObject::tr("Previous Page"));
    QObject::connect(prevButt,SIGNAL(clicked()),q_ptr,SLOT(respSwitchPage()));
    //下一页按钮
    RToolButton * nextButt = new RToolButton();
    nextButt->setObjectName(Constant::Tool_History_Next);
    nextButt->setToolTip(QObject::tr("Next Page"));
    QObject::connect(nextButt,SIGNAL(clicked()),q_ptr,SLOT(respSwitchPage()));
    //最后一页按钮
    RToolButton * lastButt = new RToolButton();
    lastButt->setObjectName(Constant::Tool_History_Last);
    lastButt->setToolTip(QObject::tr("Last Page"));
    QObject::connect(lastButt,SIGNAL(clicked()),q_ptr,SLOT(respSwitchPage()));

    filiterToolBar->setIconSize(QSize(Constant::TOOL_WIDTH+20,Constant::TOOL_HEIGHT));
    filiterToolBar->appendToolButton(setButt);
    filiterToolBar->setIconSize(QSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT));
    filiterToolBar->appendToolButton(openSearchButt);
    filiterToolBar->appendToolButton(manageButt);
    filiterToolBar->addStretch();
    filiterToolBar->setIconSize(QSize(Constant::TOOL_WIDTH+60,Constant::TOOL_HEIGHT));
    filiterToolBar->appendToolButton(dateButt);
    filiterToolBar->setIconSize(QSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT));
    filiterToolBar->appendToolButton(firstButt);
    filiterToolBar->appendToolButton(prevButt);
    filiterToolBar->appendToolButton(nextButt);
    filiterToolBar->appendToolButton(lastButt);

    searchWidget = new QWidget(allHisRecord);
    searchWidget->setObjectName("history_search");
    searchWidget->setStyleSheet("QWidget#history_search{"
                                    "background-color:rgba(232,243,253,150);}");

    QLabel *scopeLabel = new QLabel(searchWidget);
    scopeLabel->setText(QObject::tr("Scope:"));
    QComboBox *scopeCombo = new QComboBox(searchWidget);
    scopeCombo->addItem(QObject::tr("Latest onr month"));
    scopeCombo->addItem(QObject::tr("Latest three month"));
    scopeCombo->addItem(QObject::tr("Latest one year"));
    scopeCombo->addItem(QObject::tr("All"));
    scopeCombo->setCurrentIndex(3);
    scopeCombo->setFixedWidth(100);

    QLabel *contentLabel = new QLabel(searchWidget);
    contentLabel->setText(QObject::tr("Content:"));
    QLineEdit *contentEdit = new QLineEdit(searchWidget);
    contentEdit->setText("");
    contentEdit->setFixedWidth(100);

    RToolButton *sureBtn = new RToolButton(searchWidget);
    sureBtn->setText(QObject::tr("Sure"));
    sureBtn->setToolTip(QObject::tr("Sure"));

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->setContentsMargins(4,4,4,4);
    searchLayout->setSpacing(4);
    searchLayout->addWidget(scopeLabel);
    searchLayout->addWidget(scopeCombo);
    searchLayout->addWidget(contentLabel);
    searchLayout->addWidget(contentEdit);
    searchLayout->addWidget(sureBtn);
    searchLayout->addStretch();
    searchWidget->setLayout(searchLayout);
    searchWidget->setVisible(false);

    QVBoxLayout * allLayout = new QVBoxLayout;
    allLayout->setContentsMargins(0,0,0,0);
    allLayout->setSpacing(0);
    allLayout->addWidget(allView);
    allLayout->addWidget(searchWidget);
    allLayout->addWidget(filiterToolBar);
    allHisRecord->setLayout(allLayout);

    //图片类型历史消息
    QWidget * imgHisRecord = new QWidget(mainWidget);
    imgHisRecord->setAttribute(Qt::WA_DeleteOnClose);
    PreviewPage * imgPage = new PreviewPage(imgHisRecord);
    imgView = new QWebEngineView(imgHisRecord);
    imgView->setObjectName(Constant::HISTORY_WEBVIEW_IMAGE);
    imgView->setWindowFlags(imgHisRecord->windowFlags() | Qt::FramelessWindowHint);
    imgView->setPage(imgPage);
    QObject::connect(imgView,SIGNAL(loadFinished(bool)),q_ptr,SLOT(finishLoadHTML(bool)));
    imgPage->setWebChannel(m_channel);

    QVBoxLayout * imgLayout = new QVBoxLayout;
    imgLayout->setContentsMargins(0,0,0,0);
    imgLayout->setSpacing(0);
    imgLayout->addWidget(imgView);
    imgHisRecord->setLayout(imgLayout);

    //文件类型历史消息
    QWidget * fileHisRecord = new QWidget(mainWidget);
    fileHisRecord->setAttribute(Qt::WA_DeleteOnClose);
    PreviewPage * filePage = new PreviewPage(fileHisRecord);
    fileView = new QWebEngineView(fileHisRecord);
    fileView->setObjectName(Constant::HISTORY_WEBVIEW_FILE);
    fileView->setWindowFlags(fileHisRecord->windowFlags() | Qt::FramelessWindowHint);
    fileView->setPage(filePage);
    QObject::connect(fileView,SIGNAL(loadFinished(bool)),q_ptr,SLOT(finishLoadHTML(bool)));
    filePage->setWebChannel(m_channel);

    QVBoxLayout * fileLayout = new QVBoxLayout;
    fileLayout->setContentsMargins(0,0,0,0);
    fileLayout->setSpacing(0);
    fileLayout->addWidget(fileView);
    fileHisRecord->setLayout(fileLayout);

    mainWidget->addTab(allHisRecord,QObject::tr("All His"));
    mainWidget->addTab(imgHisRecord,QObject::tr("Img His"));
    mainWidget->addTab(fileHisRecord,QObject::tr("File His"));

    mainLayout->addWidget(mainWidget);
    q_ptr->setLayout(mainLayout);
}

HistoryMsgRecord::HistoryMsgRecord(QWidget *parent) :
    d_ptr(new HistoryMsgRecordPrivate(this)),
    QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(HISTORY_MIN_WIDTH,HISTORY_MIN_HEIGHT);

    d_ptr->m_bridge->setUi(this);
    //中历史记录view加载各自的html文件
    d_ptr->allView->load(QUrl("qrc:/html/resource/html/allHistory.html"));
    d_ptr->allView->show();

    d_ptr->fileView->load(QUrl("qrc:/html/resource/html/fileHisWindow.html"));
    d_ptr->fileView->show();
}

HistoryMsgRecord::~HistoryMsgRecord()
{

}

/*!
 * @brief 追加显示文本历史消息到相应页面
 * @param unitMsg 文本历史消息
 * @param pageType 显示历史消息的页面
 */
void HistoryMsgRecord::appendTextMsg(ChatInfoUnit &textMsg, HisoryType pageType)
{
    MQ_D(HistoryMsgRecord);

    RUtil::escapeLFQuote(textMsg.contents);
    RUtil::escapeDoubleQuote(textMsg.contents);
    QString t_runSCript = spliceAppendTextMsg(textMsg);

    switch (pageType) {
    case ALLHISTORY:
        d->allView->page()->runJavaScript(t_runSCript);
        break;
    case IMAGEHISTORY:
    case FILEHISTORY:
        break;
    default:
        break;
    }
}

/*!
 * @brief 追加显示图片历史消息到相应的页面
 * @param imgMsg 图片历史消息
 * @param pageType 显示历史消息的页面
 */
void HistoryMsgRecord::appendImageMsg(ChatInfoUnit &imgMsg, HisoryType pageType)
{
    MQ_D(HistoryMsgRecord);

    QString t_runSCript = spliceAppendImageMsg(imgMsg);

    switch (pageType) {
    case ALLHISTORY:
        d->allView->page()->runJavaScript(t_runSCript);
        break;
    case IMAGEHISTORY:
        d->imgView->page()->runJavaScript(t_runSCript);
        break;
    case FILEHISTORY:
        d->fileView->page()->runJavaScript(t_runSCript);
        break;
    default:
        break;
    }
}

/*!
 * @brief 追加显示文件历史消息到相应页面
 * @param fileMsg 文件历史消息
 * @param pageType 显示历史消息的页面
 */
void HistoryMsgRecord::appendFileMsg(ChatInfoUnit &fileMsg, HisoryType pageType)
{
    MQ_D(HistoryMsgRecord);

    QString t_runSCript = spliceAppendFileMsg(fileMsg);

    switch (pageType) {
    case ALLHISTORY:
        d->allView->page()->runJavaScript(t_runSCript);
        break;
    case IMAGEHISTORY:
        break;
    case FILEHISTORY:
        d->fileView->page()->runJavaScript(t_runSCript);
        break;
    default:
        break;
    }
}

//TODO LYS-20180814 根据语音消息存储结构实现显示语音历史消息
void HistoryMsgRecord::appendVoiceMsg()
{

}

/*!
 * @brief 追加显示提示消息到相应页面
 * @param notice 提示消息内容
 * @param pageType 显示历史消息的页面
 * @param type 提示信息类型
 */
void HistoryMsgRecord::appendNoticeMsg(QString notice, HisoryType pageType, NoticeType type)
{
    MQ_D(HistoryMsgRecord);

    QString t_runSCript = spliceAppendNoticeMsg(notice,type);

    switch (pageType) {
    case ALLHISTORY:
        d->allView->page()->runJavaScript(t_runSCript);
        break;
    case IMAGEHISTORY:
    case FILEHISTORY:
        break;
    default:
        break;
    }
}

/*!
 * @brief 追加显示分割日期到相应页面
 * @param curDate 需要显示的分割日期
 */
void HistoryMsgRecord::appendDateMsg(QDate curDate,HisoryType pageType)
{
    MQ_D(HistoryMsgRecord);

    QString t_runSCript = spliceAppendDateLine(curDate);

    switch (pageType) {
    case ALLHISTORY:
        d->allView->page()->runJavaScript(t_runSCript);
        break;
    case IMAGEHISTORY:
    case FILEHISTORY:
        break;
    default:
        break;
    }
}

/*!
 * @brief 清空历史记录页面中的内容
 * @param pageType
 */
void HistoryMsgRecord::clearShow(HistoryMsgRecord::HisoryType pageType)
{
    MQ_D(HistoryMsgRecord);

    QString t_runSCript = QString("removeAllRecord()");

    switch (pageType) {
    case ALLHISTORY:
        d->allView->page()->runJavaScript(t_runSCript);
        break;
    case IMAGEHISTORY:
        d->imgView->page()->runJavaScript(t_runSCript);
        break;
    case FILEHISTORY:
        d->fileView->page()->runJavaScript(t_runSCript);
        break;
    default:
        break;
    }
}

/*!
 * @brief 设置联系人基本信息
 * @param info 联系人基本信息
 */
void HistoryMsgRecord::setUserInfo(const SimpleUserInfo &info)
{
    MQ_D(HistoryMsgRecord);

    d->m_userInfo = info;
}

/*!
 * @brief 打开文件
 * @param filePath 目标文件全路径
 */
void HistoryMsgRecord::openTargetFile(QString filePath)
{
    QProcess::execute(QString("explorer %1").arg(QDir::toNativeSeparators(filePath)));
}

/*!
 * @brief 打开文件夹
 * @param folderPath 文件夹全路径
 */
void HistoryMsgRecord::openTargetFolder(QString filePath)
{
    RUtil::showInExplorer(filePath);
}

void HistoryMsgRecord::finishLoadHTML(bool flag)
{
    Q_UNUSED(flag);
    MQ_D(HistoryMsgRecord);

    QString t_objName = QObject::sender()->objectName();
    if(t_objName == Constant::HISTORY_WEBVIEW_ALL)
    {
        setFontIconFilePath(ALLHISTORY);
        RSingleton<ChatMsgProcess>::instance()->appendC2CAllHistoryQueryTask(d->m_userInfo.accountId,0,60);
    }
    else if(t_objName == Constant::HISTORY_WEBVIEW_IMAGE)
    {
        setFontIconFilePath(IMAGEHISTORY);
    }
    else if(t_objName == Constant::HISTORY_WEBVIEW_FILE)
    {
        setFontIconFilePath(FILEHISTORY);
    }
}

void HistoryMsgRecord::respOpenSearch(bool flag)
{
    MQ_D(HistoryMsgRecord);

    d->searchWidget->setVisible(flag);

}

void HistoryMsgRecord::respChooseDate()
{
    MQ_D(HistoryMsgRecord);

    QPoint btnPos = d->dateButt->mapToGlobal(QPoint(0,0));
    if(!d->chooseDate->isVisible())
    {
        d->chooseDate->setVisible(true);
    }
    int height = d->chooseDate->height();
    int width = d->chooseDate->width();
    d->chooseDate->setGeometry(btnPos.x(),btnPos.y()-height,width,height);
}

void HistoryMsgRecord::respSetDate(const QDate &date)
{
    MQ_D(HistoryMsgRecord);

    d->dateButt->setText(date.toString("yyyy-MM-dd"));
    d->chooseDate->setVisible(false);
}

void HistoryMsgRecord::respSwitchPage()
{
    appendDateMsg(QDate::currentDate());
    ChatInfoUnit tmp;
    tmp.accountId = "liangyunshan";
    tmp.contents = "Hello World!";
    tmp.contentType = MSG_TEXT_TEXT;
    tmp.dtime = qint64(1533548053323);
    appendTextMsg(tmp);

}

void HistoryMsgRecord::setFontIconFilePath(HisoryType pageType)
{
    MQ_D(HistoryMsgRecord);

    QString t_currentPath = QDir::currentPath();
    t_currentPath = QDir::fromNativeSeparators(t_currentPath);
    QString t_runSCript = QString("setAbsoulteFontIconHerf('%1')").arg(t_currentPath);
    switch (pageType) {
    case ALLHISTORY:
        d->allView->page()->runJavaScript(t_runSCript);
        break;
    case IMAGEHISTORY:
        d->imgView->page()->runJavaScript(t_runSCript);
        break;
    case FILEHISTORY:
        d->fileView->page()->runJavaScript(t_runSCript);
        break;
    default:
        break;
    }
}

/*!
 * @brief 根据消息记录拼接插入文本消息js脚本
 * @param unitMsg 消息记录
 * @return 插入文本消息js脚本
 */
QString HistoryMsgRecord::spliceAppendTextMsg(const ChatInfoUnit &unitMsg)
{
    QJsonObject obj;
    int target = 0;
    if(G_User->BaseInfo().accountId == unitMsg.accountId)
    {
        target = 1;
    }
    QTime t_curTime = RUtil::addMSecsToEpoch(unitMsg.dtime).time();
    obj.insert("name",unitMsg.accountId);
    obj.insert("time",t_curTime.toString("hh:mm:ss"));
    obj.insert("message",unitMsg.contents);
    obj.insert("target",target);

    QString jsonStr = QString(QJsonDocument(obj).toJson()).replace("\n","");
    QString t_script;
    t_script =QString("appendTextHistory('%1')").arg(jsonStr);
    return t_script;
}

/*!
 * @brief 根据消息记录拼接插入图片消息js脚本
 * @param unitMsg 消息记录
 * @return 插入图片消息js脚本
 */
QString HistoryMsgRecord::spliceAppendImageMsg(const ChatInfoUnit &unitMsg)
{
    QJsonObject obj;
    int target = 0;
    if(G_User->BaseInfo().accountId == unitMsg.accountId)
    {
        target = 1;
    }
    QTime t_curTime = RUtil::addMSecsToEpoch(unitMsg.dtime).time();
    obj.insert("name",unitMsg.accountId);
    obj.insert("time",t_curTime.toString("hh:mm:ss"));
    obj.insert("message",unitMsg.contents);
    obj.insert("target",target);

    QString jsonStr = QString(QJsonDocument(obj).toJson()).replace("\n","");
    QString t_script;
    t_script =QString("appendImageHistory('%1')").arg(jsonStr);
    return t_script;
}

/*!
 * @brief 根据消息记录拼接插入文件消息js脚本
 * @param unitMsg 消息记录
 * @return 插入文件消息js脚本
 */
QString HistoryMsgRecord::spliceAppendFileMsg(const ChatInfoUnit &unitMsg)
{
    QJsonObject obj;
    int target = 0; //默认为自己发送
    int t_fileType = FILE_TXT;

    //文件大小
    QFileInfo t_info(unitMsg.contents);
    QString t_fileName = QString();
    if(t_info.isFile())
    {
        t_fileName = t_info.fileName();
    }
    else
    {
        QDir t_dir(unitMsg.contents);
        t_fileName = t_dir.dirName();
        t_fileType = FILE_FOLDER;
    }
    int t_size = t_info.size();
    QString t_fileSize = QString();
    if(t_size < 1024)
    {
        QString t_showSize = QString::number(t_size,'f',2);
        t_fileSize = QString("(%1B)").arg(t_showSize);
    }
    else if(t_size > 1024)
    {
        QString t_showSize = QString::number(t_size/1024,'f',2);
        t_fileSize = QString("(%1KB)").arg(t_showSize);
    }
    else if(t_size > 1024*1024)
    {
        QString t_showSize = QString::number(t_size/(1024*1024),'f',2);
        t_fileSize = QString("(%1MB)").arg(t_showSize);
    }

    if(G_User->BaseInfo().accountId == unitMsg.accountId)
    {
        target = 1; //修改为接收自其它
    }
    QTime t_curTime = RUtil::addMSecsToEpoch(unitMsg.dtime).time();
    QDate t_curDate = RUtil::addMSecsToEpoch(unitMsg.dtime).date();
    obj.insert("name",unitMsg.accountId);
    obj.insert("filePath",unitMsg.contents);
    obj.insert("fileName",t_fileName);
    obj.insert("fileType",t_fileType);
    obj.insert("fileSize",t_fileSize);
    obj.insert("date",t_curDate.toString("MM-dd"));
    obj.insert("time",t_curTime.toString("hh:mm:ss"));
    obj.insert("target",target);
    obj.insert("resultType",1);

    QString jsonStr = QString(QJsonDocument(obj).toJson()).replace("\n","");
    QString t_script;
    t_script =QString("appendFileHistory('%1')").arg(jsonStr);
    return t_script;
}

QString HistoryMsgRecord::spliceAppendVoiceMsg(const QString &voicePath)
{
    QJsonObject obj;
    int target = 0;
    QTime t_curTime = QTime::currentTime();
    obj.insert("name","Admin");
    obj.insert("time",t_curTime.toString("hh:mm:ss"));
    obj.insert("voice",voicePath);
    obj.insert("length",6);
    obj.insert("target",target);

    QString jsonStr = QString(QJsonDocument(obj).toJson()).replace("\n","");
    QString t_script;
    t_script =QString("appendVoiceHistory('%1')").arg(jsonStr);
    return t_script;
}

/*!
 * @brief 拼接生成插入提示历史消息js脚本
 * @param notice
 * @param type
 * @return
 */
QString HistoryMsgRecord::spliceAppendNoticeMsg(const QString &notice,NoticeType type)
{
    QString t_script;
    t_script = QString("appendNoticeHistory('%1',%2)").arg(notice).arg(type);
    return t_script;
}

/*!
 * @brief 根据给定日期拼接插入日期分界线js脚本
 * @param date 给定日期
 * @return 拼接插入日期分界线js脚本
 */
QString HistoryMsgRecord::spliceAppendDateLine(const QDate &date)
{
    QString t_script;
    t_script = QString("appendSeparateDate('%1')").arg(date.toString("yyyy-MM-dd"));
    return t_script;
}
