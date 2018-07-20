#include "maindialog.h"

#include <QDesktopWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QHBoxLayout>
#include <QApplication>
#include <QMessageBox>
#include <QToolButton>
#include <QPropertyAnimation>
#include <QSharedPointer>
#include <QAudioFormat>
#include <QDir>

#include "systemtrayicon.h"
#include "Util/rutil.h"
#include "constants.h"
#include "head.h"
#include "global.h"
#include "toolbar.h"
#include "panelbottomtoolbar.h"
#include "panelcontentarea.h"
#include "paneltoparea.h"
#include "actionmanager/actionmanager.h"
#include "Util/imagemanager.h"
#include "toolbox/toolitem.h"
#include "util/rsingleton.h"
#include "actionmanager/shortcutsettings.h"
#include "editpersoninfowindow.h"
#include "messdiapatch.h"
#include "widget/rmessagebox.h"
#include "user/userclient.h"
#include "user/userfriendcontainer.h"
#include "panelpersonpage.h"
#include "media/mediaplayer.h"
#include "user/user.h"
#include "thread/taskmanager.h"
#include "media/audioinput.h"
#include "media/audiooutput.h"
#include "others/msgqueuemanager.h"

#include "itemhoverinfo.h"

#include "sql/databasemanager.h"
#include "sql/sqlprocess.h"
#include "screenshot.h"

#define PANEL_MARGIN 20
#define PANEL_HIDEMARGIN 1
#define Panel_ANDURATION 200

class MainDialogPrivate : public GlobalData<MainDialog>
{
    Q_DECLARE_PUBLIC(MainDialog)

    enum Direction
    {
        None,
        Up,
        Right,
        Down,
        Left
    };

    MainDialogPrivate(MainDialog *q):q_ptr(q)
    {
        editWindow = NULL;
        m_bIsAutoHide = false;
        m_enDriection = None;
    }

private:
    QWidget * MainPanel;
    QWidget * TopBar;
    QWidget * Conent;
    QWidget * ToolBarWidget;

    ToolBar * toolBar;
    PanelBottomToolBar * bottomToolBar;
    PanelContentArea * panelContentArea;
    PanelTopArea * panelTopArea;

    QMap<ToolItem * ,ItemHoverInfo *> hoverInfos;
    EditPersonInfoWindow * editWindow;

    bool m_bIsAutoHide;
    Direction m_enDriection;

    MainDialog * q_ptr;
};

MainDialog * MainDialog::dialog = NULL;

MainDialog::MainDialog(QWidget *parent) :
    d_ptr(new MainDialogPrivate(this)),
    Widget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(Constant::MAIN_PANEL_MIN_WIDTH,Constant::MAIN_PANEL_MIN_HEIGHT);
    setMaximumWidth(Constant::MAIN_PANEL_MAX_WIDTH);
    setMaximumHeight(qApp->desktop()->screen()->height());

    RSingleton<Subject>::instance()->attach(this);
    dialog = this;
    ScreenShot::instance();

    initSqlDatabase();
    initMedia();
    initWidget();

    connect(MessDiapatch::instance(),SIGNAL(errorGroupingOperate(OperateGrouping)),this,SLOT(errorGroupingOperate(OperateGrouping)));
    connect(MessDiapatch::instance(),SIGNAL(screenChange()),this,SLOT(screenChanged()));

    //716
    connect(MessDiapatch::instance(),SIGNAL(recvText(TextRequest)),this,SLOT(procRecvText(TextRequest)));
    connect(MessDiapatch::instance(),SIGNAL(recvTextReply(TextReply)),this,SLOT(procRecvServerTextReply(TextReply)));

}

MainDialog::~MainDialog()
{
    MQ_D(MainDialog);
    RSingleton<Subject>::instance()->detach(this);

    RSingleton<TaskManager>::instance()->removeAll();

    if(d->editWindow){
        delete d->editWindow;
    }

    RSingleton<UserManager>::instance()->closeAllClientWindow();

    RSingleton<ShortcutSettings>::instance()->save();
}

MainDialog *MainDialog::instance()
{
    return dialog;
}

void MainDialog::onMessage(MessageType type)
{
    switch(type)
    {
        case MESS_SETTINGS:
            {
                makeWindowFront(G_User->systemSettings()->keepFront);
                blockAutoHidePanel();
            }
            break;
        case MESS_SCREEN_CHANGE:
            {
            //如果运行这一句，在左右屏幕之间拖动聊天界面的时候，会将已经隐藏到桌边的主面板显示到固定的1屏幕位置
//                changeGeometry(geometry());
            }
            break;
         case MESS_USER_OFF_LINE:
            {
                G_User->setTextOnline(false);
                G_User->setFileOnline(false);
                G_User->setLogin(false);
                RSingleton<TaskManager>::instance()->removeAll();
            }
            break;
        default:break;
    }
}

/*!
 * @brief 设置面板中用户的登录状态显示
 * @param[in] state:OnlineStatus，用户登录状态
 * @return 无
 */
void MainDialog::setLogInState(OnlineStatus state)
{
    MQ_D(MainDialog);
    d->panelTopArea->setState(state);
}

void MainDialog::resizeEvent(QResizeEvent *)
{
    updateWidgetGeometry();
}

void MainDialog::closeEvent(QCloseEvent *)
{
    writeSettings();
    //Yang 延时退出，否则ini文件无法被更新至本地
    QTimer::singleShot(50,qApp,SLOT(quit()));
}


void MainDialog::leaveEvent(QEvent *event)
{
    MQ_D(MainDialog);
    isAutoHide();
    if(d->m_bIsAutoHide && G_User->systemSettings()->hidePanel)
    {
        hidePanel();
    }
    QWidget::leaveEvent(event);
}

void MainDialog::enterEvent(QEvent *event)
{
    MQ_D(MainDialog);
    if(d->m_bIsAutoHide && G_User->systemSettings()->hidePanel)
    {
        showPanel();
    }
    QWidget::enterEvent(event);
}

void MainDialog::updateWidgetGeometry()
{
    MQ_D(MainDialog);
    QLayout * lay = layout();
    int right = 0;
    int left  = 0;
    if(lay)
    {
        QMargins margins = lay->contentsMargins();
        right = margins.right();
        left = margins.left();
    }
    d->toolBar->setGeometry(left,0,this->width() - right * 3,Constant::TOOL_HEIGHT);
}

void MainDialog::closeWindow()
{
    if(G_User->systemSettings()->exitSystem)
        this->close();
    else
        this->hide();
}

void MainDialog::makeWindowFront(bool flag)
{
    Qt::WindowFlags flags = windowFlags();

    if(flag)
    {
        setWindowFlags(flags | Qt::WindowStaysOnTopHint);
        ActionManager::instance()->toolButton(Id(Constant::TOOL_PANEL_FRONT))->setToolTip(tr("Unstick"));
    }
    else
    {
        setWindowFlags(flags & ~Qt::WindowStaysOnTopHint);
        ActionManager::instance()->toolButton(Id(Constant::TOOL_PANEL_FRONT))->setToolTip(tr("Stick"));
    }

    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_TOPHINT,flag);

    show();
}

void MainDialog::blockAutoHidePanel()
{
    MQ_D(MainDialog);
    isAutoHide();
    if(!G_User->systemSettings()->hidePanel)
    {
        if(d->m_bIsAutoHide)
        {
            moveToDesktop(d->m_enDriection);
        }
    }
    else
    {
        if(d->m_bIsAutoHide)
        {
            hidePanel();
        }
    }
}

void MainDialog::showHoverItem(bool flag, ToolItem * item)
{
    MQ_D(MainDialog);
    if(flag)
    {
        UserClient * client = RSingleton<UserManager>::instance()->client(item);
        if(!client)
        {
            return;
        }
        ItemHoverInfo * info = new ItemHoverInfo;
        info->setSimpleUserInfo(client->simpleUserInfo);
        info->fadein(item->mapToGlobal(QPoint(0,0)));
        d->hoverInfos.insert(item,info);
    }
    else
    {
        ItemHoverInfo  *info = d->hoverInfos.value(item);
        if(info)
        {
            d->hoverInfos.remove(item);
            info->fadeout();
        }
    }
}

void MainDialog::showPersonalEditWindow()
{
    MQ_D(MainDialog);
    if(!d->editWindow)
    {
        d->editWindow = new EditPersonInfoWindow();
        connect(d->editWindow,SIGNAL(destroyed(QObject*)),this,SLOT(updateEditInstance()));
    }
    if(d->editWindow->isMinimized())
    {
        d->editWindow->showNormal();
    }
    else
    {
        d->editWindow->show();
    }
}

void MainDialog::updateEditInstance()
{
   MQ_D(MainDialog);
   d->editWindow = NULL;
}

void MainDialog::errorGroupingOperate(OperateGrouping type)
{
    QString errorInfo;
    switch(type)
    {
        case GROUPING_CREATE:
            {
                errorInfo = QObject::tr("Create group failed!");
            }
            break;
        case GROUPING_RENAME:
            {
                errorInfo = QObject::tr("Rename group failed!");
            }
            break;
        case GROUPING_DELETE:
            {
                errorInfo = QObject::tr("Delete group failed!");
            }
            break;
        default:break;
    }

    RMessageBox::warning(this,"warning",errorInfo,RMessageBox::Yes);
}

void MainDialog::initWidget()
{
    MQ_D(MainDialog);

    readSettings();

    d->MainPanel = new QWidget(this);
    d->MainPanel->setObjectName("MainPanel");

    setContentWidget(d->MainPanel);

    d->TopBar = new QWidget(this);
    d->TopBar->setFixedHeight(150);
    d->TopBar->setObjectName("TopBar");

    d->Conent = new QWidget(this);
    d->Conent->setObjectName("Conent");

    d->ToolBarWidget = new QWidget(this);
    d->ToolBarWidget->setObjectName("ToolBarWidget");

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(d->TopBar);
    mainLayout->addWidget(d->Conent);
    mainLayout->addWidget(d->ToolBarWidget);

    d->MainPanel->setLayout(mainLayout);

    connect(SystemTrayIcon::instance(),SIGNAL(quitApp()),this,SLOT(closeWindow()));
    connect(SystemTrayIcon::instance(),SIGNAL(showMainPanel()),this,SLOT(showNormal()));

    d->toolBar = new ToolBar(d->MainPanel);
    d->toolBar->setToolFlags(ToolBar::TOOL_ICON|ToolBar::TOOL_MIN|ToolBar::TOOL_CLOSE|ToolBar::TOOL_SPACER);
    connect(d->toolBar,SIGNAL(minimumWindow()),this,SLOT(hide()));
    connect(d->toolBar,SIGNAL(closeWindow()),this,SLOT(closeWindow()));

    d->toolBar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM,ImageManager::ICON_16));

    RToolButton * frontButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_FRONT,this,SLOT(makeWindowFront(bool)),true);

    d->toolBar->insertToolButton(frontButton,Constant::TOOL_MIN);

    //读取个人配置信息设置系统功能
    makeWindowFront(G_User->systemSettings()->keepFront);
    blockAutoHidePanel();

    d->panelTopArea = new PanelTopArea(d->TopBar);
    QHBoxLayout * topAreaLayout = new QHBoxLayout();
    topAreaLayout->setContentsMargins(0,0,0,0);
    topAreaLayout->setSpacing(0);
    topAreaLayout->addWidget(d->panelTopArea);
    d->TopBar->setLayout(topAreaLayout);

    //中部内容区
    d->panelContentArea = new PanelContentArea(d->Conent);
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(d->panelContentArea);
    d->Conent->setLayout(contentLayout);

    //底部工具栏
    d->bottomToolBar = new PanelBottomToolBar(d->ToolBarWidget);
    QHBoxLayout * bottomToolLayout = new QHBoxLayout();
    bottomToolLayout->setContentsMargins(0,0,0,0);
    bottomToolLayout->setSpacing(0);
    bottomToolLayout->addWidget(d->bottomToolBar);
    d->ToolBarWidget->setLayout(bottomToolLayout);

    QTimer::singleShot(0,this,SLOT(updateWidgetGeometry()));
}

#define SCALE_ZOOMIN_FACTOR 1.2
#define SCALE_ZOOMOUT_FACTOR 0.75

/*!
 * @brief 解析配置文件，根据配置文件中的尺寸设置窗口位置
 * @param[in] 无
 * @return 无
 * @note 在设置屏幕尺寸时需考虑在多屏与单屏之间的切换显示，假设存在主屏A，和扩展屏B； @n
 *      1.在复制模式中，不会受影响； @n
 *      2.在扩展模式中，a.程序运行在A上，动态加入B，此时不受影响； @n
 *                   b.有A、B两屏，程序运行在B上，移除B，程序需动态切换至A @n
 *                   c.有A、B两屏，程序运行在B上，重新调整显示器排列顺序，如将水平排列改为垂直排列，程序需动态切换 @n
 *      3.仅在A或仅在B模式：程序不受影响 @n
 */
void MainDialog::readSettings()
{
    QSettings * settings = G_User->getSettings();
    settings->beginGroup(Constant::USER_BASIC_GROUP);
    if(!settings->value(Constant::USER_BASIC_X).isValid() || !settings->value(Constant::USER_BASIC_Y).isValid()
            ||!settings->value(Constant::USER_BASIC_WIDTH).isValid() ||!settings->value(Constant::USER_BASIC_HEIGHT).isValid())
    {
        QRect rect = qApp->desktop()->screen()->geometry();

        int tmpWidth = Constant::MAIN_PANEL_MIN_WIDTH * SCALE_ZOOMIN_FACTOR;
        int tmpHeight = rect.height() * SCALE_ZOOMOUT_FACTOR;

        settings->setValue(Constant::USER_BASIC_X,rect.width() - tmpWidth - PANEL_MARGIN);
        settings->setValue(Constant::USER_BASIC_Y,PANEL_MARGIN);
        settings->setValue(Constant::USER_BASIC_WIDTH,tmpWidth);
        settings->setValue(Constant::USER_BASIC_HEIGHT,tmpHeight);
    }

    int x = settings->value(Constant::USER_BASIC_X).toInt();
    int y = settings->value(Constant::USER_BASIC_Y).toInt();
    int w = settings->value(Constant::USER_BASIC_WIDTH).toInt();
    int h = settings->value(Constant::USER_BASIC_HEIGHT).toInt();
    settings->endGroup();

    settings->beginGroup(Constant::USER_SETTING_GROUP);

    G_User->systemSettings()->autoStartUp = settings->value(Constant::USER_SETTING_AUTO_STARTUP,false).toBool();
    G_User->systemSettings()->autoLogin = settings->value(Constant::USER_SETTING_AUTO_LOGIN,false).toBool();

    G_User->systemSettings()->keepFront = settings->value(Constant::USER_SETTING_TOPHINT,false).toBool();
    G_User->systemSettings()->exitSystem = settings->value(Constant::USER_SETTING_EXIT_SYSTEM,true).toBool();
    G_User->systemSettings()->hidePanel = settings->value(Constant::USER_SETTING_HIDEPANEL,true).toBool();
    G_User->systemSettings()->trayIcon = settings->value(Constant::SETTING_TRAYICON,true).toBool();

    G_User->systemSettings()->windowShaking = settings->value(Constant::USER_SETTING_WINDOW_SHAKE,true).toBool();
    G_User->systemSettings()->soundAvailable = settings->value(Constant::USER_SETTING_SOUND_AVAILABLE,true).toBool();
    G_User->systemSettings()->lockCheck = settings->value(Constant::USER_SETTING_SYSTEM_LOCK,false).toBool();
    G_User->systemSettings()->recordCheck = settings->value(Constant::USER_SETTING_EXIT_DELRECORD,false).toBool();
    G_User->systemSettings()->encryptionCheck = settings->value(Constant::USER_SETTING_TEXT_ENCRYPTION,false).toBool();
    G_User->systemSettings()->compressCheck = settings->value(Constant::USER_SETTING_TEXT_COMPRESSION,false).toBool();

    settings->endGroup();

    changeGeometry(x,y,w,h);
}

/*!
 * @brief 响应屏幕尺寸改变事件
 * @param[in] 无
 * @return 无
 * @warning 目前不能正确响应多屏幕排序而产生的事件。
 */
void MainDialog::screenChanged()
{
    RSingleton<Subject>::instance()->notify(MESS_SCREEN_CHANGE);
}

void MainDialog::procRecvText(TextRequest response)
{
    UserClient * client = RSingleton<UserManager>::instance()->client(response.otherSideId);
    if(client)
    {
        client->procRecvContent(response);
    }
}

/*!
 * @brief 接收系统消息反馈
 * @param[in] reply 消息回复
 * @return 无
 * @note 接收系统推送的消息反馈后，根据反馈的类型不同，执行相应的操作: @n
 *      1.APPLY_SYSTEM:将聊天信息存入对应用户的db; @n
 *      2.APPLY_CONFIRM:显示接收提示(弹窗或其它方式显示); @n
 *      3.APPLY_RECEIPT:更新回执状态(已读/未读);
 */
void MainDialog::procRecvServerTextReply(TextReply reply)
{
    //TODO 尚超：因为需要实现信息的已读和未读状态控制，我们计划将信息从数据库中读取出来后，将对应流水号的状态修改为已读，同时更新数据库
    //从内存缓存中读取流水号的方法已经不适应现有的已读未读状态控制的场景
    //如果需要翻看以前的代码，请使用版本管理工具查看
     UserClient * client = RSingleton<UserManager>::instance()->client(QString::number(reply.wSourceAddr));
     if(client && client->chatPersonWidget != NULL)
     {
         client->procRecvServerTextReply(reply);
     }
}

/*!
 * @brief 调整程序显示位置
 * @param[in] x 原始屏幕坐标X
 * @param[in] y 原始屏幕坐标Y
 * @param[in] w 原始程序宽度
 * @param[in] h 原始程序高度
 * @return 无
 * @note 获取当前外接的显示器数量，获取每块显示器的显示范围。将原始的尺寸与每块显示的显示范围进行比较，@n
 *       若在显示器的显示范围之内，则将程序设置在此屏幕上；若不在此显示范围内，则将程序设置在默认的屏幕上。 @n
 *       若A为主屏，B为辅屏，两者的尺寸均为W,H，在排列上有以下几种情况: @n
 *       1.AB排列：水平范围：[0,2W]、竖直范围：[0,H] @n
 *       2.BA排列：水平范围：[-W,W]、竖直范围：[0,H] @n
 *       3.A/B排列：水平范围：[0,W]、竖直范围：[0,2H] @n
 *       4.B/A排列：水平范围：[0,W]、竖直范围：[-H,H] @n
 */
void MainDialog::changeGeometry(int x, int y, int w, int h)
{
    MQ_D(MainDialog);
    bool foundScreen = false;


    //【1】查找上一次的位置是否在当前显示中可用，若可用，则可直接设置
    int screenSize = qApp->desktop()->screenCount();
    for(int i = 0; i < screenSize; i++)
    {
        QRect screenRect = qApp->desktop()->screenGeometry(i);
        int minX = screenRect.x();
        int maxX = screenRect.x() + screenRect.width();
        int minY = screenRect.y();
        int maxY = screenRect.y() + screenRect.height();

        if(x >= minX && x <= maxX && y >= minY && y <= maxY)
        {
            this->setGeometry(x,y,w,h);
            foundScreen = true;
            break;
        }
    }

    //【2】若不可用，则将位置置为默认窗口显示
    if(!foundScreen)
    {
        QRect defaultRect = qApp->desktop()->screen()->rect();
        setGeometry(defaultRect.width() - w - 10,20,w,h);

        d->m_enDriection = d->None;
        d->m_bIsAutoHide = false;
    }
}

void MainDialog::changeGeometry(QRect rect)
{
    changeGeometry(rect.x(),rect.y(),rect.width(),rect.height());
}

void MainDialog::moveToDesktop(int direction)
{
    MQ_D(MainDialog);
    QRect t_rect = this->geometry();
    QPropertyAnimation *t_animation = new QPropertyAnimation(this, "geometry");
    t_animation->setDuration(Panel_ANDURATION);
    t_animation->setStartValue(t_rect);
    QRect t_endRect;

    switch (direction)
    {
        case d->Left:
            t_endRect = QRect(t_rect.x()+t_rect.width(),t_rect.y(),t_rect.width(),t_rect.height());
            break;

        case d->Up:
            t_endRect = QRect(t_rect.x(),t_rect.y()+t_rect.height(),t_rect.width(),t_rect.height());
            break;

        case d->Right:
            t_endRect = QRect(t_rect.x()-t_rect.width(),t_rect.y(),t_rect.width(),t_rect.height());
            break;

        default:
            t_endRect = t_rect;
            break;
    }
    t_animation->setEndValue(t_endRect);
    t_animation->start(QAbstractAnimation::DeleteWhenStopped);
    Q_UNUSED(d);
}

void MainDialog::writeSettings()
{
    QRect rect = this->geometry();
    QSettings * settings = G_User->getSettings();
    settings->beginGroup(Constant::USER_BASIC_GROUP);
    settings->setValue(Constant::USER_BASIC_X,rect.x());
    settings->setValue(Constant::USER_BASIC_Y,rect.y());
    settings->setValue(Constant::USER_BASIC_WIDTH,rect.width());
    settings->setValue(Constant::USER_BASIC_HEIGHT,rect.height());
    settings->endGroup();
}

/*!
 * @brief 初始化数据库连接
 * @details 从配置文件中读取数据库连接信息，若没有填写对应的信息，则启用系统默认信息
 */
void MainDialog::initSqlDatabase()
{
    QSharedPointer<DatabaseManager> db_ptr(new DatabaseManager());

    QString type = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_TYPE,Constant::DEFAULT_SQL_TYPE).toString();
    QString hostName = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_HOSTNAME,Constant::DEFAULT_SQL_HOST).toString();
    QString databaseName = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_DATABASE,Constant::DEFAULT_SQL_DATABASE).toString();
    QString userName = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_USER,Constant::DEFAULT_SQL_USER).toString();
    int port = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_PORT,Constant::DEFAULT_SQL_PORT).toString().toInt();

    bool useInnerPass = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_INNER_PASSWORD,Constant::DEFAULT_SQL_INNER_PASS).toBool();
    QString password;
    if(useInnerPass){
        password = Constant::DEFAULT_SQL_PASSWORD;
    }else{
        RUtil::globalSettings()->beginGroup(Constant::SYSTEM_DB);
        password = RUtil::globalSettings()->value(Constant::SYSTEM_DB_PASS,"").toString();
        RUtil::globalSettings()->endGroup();
    }

    if(db_ptr->testSupportDB(type)){
        if(type.toUpper().contains(SQL_SQLITE)){
            if(!databaseName.contains("\\.db")){
                databaseName += ".db";
            }
            databaseName = G_User->getUserDatabasePath() +QDir::separator() + databaseName;
        }
        db_ptr->setConnectInfo(hostName,databaseName,userName,password,port);
        db_ptr->setDatabaseType(type);
        Database * chatDatabase = db_ptr->newDatabase(RUtil::UUID());
        if(chatDatabase == nullptr){
            RMessageBox::warning(this,tr("warning"),tr("Open chat message database error! \n please check database config."),RMessageBox::Yes);
            return;
        }
        G_User->setDatabase(chatDatabase);

        if(!RSingleton<SQLProcess>::instance()->createTableIfNotExists(chatDatabase))
            RMessageBox::warning(this,tr("warning"),tr("Database tables create error!"),RMessageBox::Yes);
    }else{
        RMessageBox::warning(this,tr("warning"),tr("Don't support database type [%1]!").arg(type),RMessageBox::Yes);
    }
}

void MainDialog::initMedia()
{
    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    RSingleton<AudioInput>::instance()->setAudioFormat(format);
    RSingleton<AudioOutput>::instance()->setAudioFormat(format);
}

/*!
 * @brief 判断面板贴边隐藏的方向
 * @param[in] 无
 * @return 无
 * @note 在检测时，需考虑当前显示屏幕的数量。根据屏幕的组合不同，动态的调整
 */
void MainDialog::isAutoHide()
{
    MQ_D(MainDialog);
    QPoint t_pos = this->pos();
    d->m_bIsAutoHide = true;

    QRect screenGeometry = RUtil::screenGeometry();

    if(t_pos.x()< screenGeometry.left() + PANEL_HIDEMARGIN + shadowWidth())
    {
        d->m_enDriection = d->Left;
    }
    else if(t_pos.y() < screenGeometry.top() + PANEL_HIDEMARGIN + shadowWidth())
    {
        d->m_enDriection = d->Up;
    }
    else if(this->pos().x() + this->width() > (screenGeometry.left() + screenGeometry.width()) - PANEL_HIDEMARGIN - shadowWidth())
    {
        d->m_enDriection = d->Right;
    }
    else
    {
        d->m_enDriection = d->None;
        d->m_bIsAutoHide = false;
    }
}

/*!
 * @brief 动画隐藏面板
 * @param[in] 无
 * @return 无
 * @note 目前直接获取width()其宽度是不准确的，因包含了两个的shadowWidth.
 */
void MainDialog:: hidePanel()
{
    MQ_D(MainDialog);
    int t_xValue,t_yValue,t_width,t_height;
    t_xValue = 0;
    t_yValue = 0;
    t_width = this->width();
    t_height = this->height();

    QRect screenGeometry = RUtil::screenGeometry();

    QPropertyAnimation *t_animation = new QPropertyAnimation(this, "geometry");
    t_animation->setDuration(Panel_ANDURATION);
    t_animation->setStartValue(QRect(this->pos(), this->size()));

    QRect t_endRect;
    if(d->m_enDriection & d->Up)
    {
        t_xValue = this->x();
        t_yValue = screenGeometry.top() - this->height() + PANEL_HIDEMARGIN + shadowWidth();
        t_endRect = QRect(t_xValue, t_yValue, t_width, t_height);
    }
    else if(d->m_enDriection & d->Left)
    {
        t_xValue = screenGeometry.left() - this->width() + PANEL_HIDEMARGIN + shadowWidth();
        t_yValue = this->y();
        t_endRect = QRect(t_xValue, t_yValue, t_width, t_height);
    }
    else if(d->m_enDriection & d->Right)
    {
        t_xValue = (screenGeometry.left() + screenGeometry.width()) - 2*PANEL_HIDEMARGIN - shadowWidth();
        t_yValue = this->y();
        t_endRect = QRect(t_xValue, t_yValue, t_width, t_height);
    }
    else
    {
        t_endRect = this->rect();
    }

    t_animation->setEndValue(t_endRect);
    t_animation->start(QAbstractAnimation::DeleteWhenStopped);
}

/*!
 * @brief 动画显示面板
 * @param[in] 无
 * @return 无
 */
void MainDialog::showPanel()
{
    MQ_D(MainDialog);
    int t_xValue,t_yValue,t_width,t_height;
    t_xValue = 0;
    t_yValue = 0;
    t_width = this->width();
    t_height = this->height();

    QRect screenGeometry = RUtil::screenGeometry();

    QPropertyAnimation *t_animation = new QPropertyAnimation(this, "geometry");
    t_animation->setDuration(Panel_ANDURATION);
    t_animation->setStartValue(QRect(this->pos(), this->size()));

    QRect t_endRect;
    if (d->m_enDriection & d->Up)
    {
        t_xValue = this->x();
        t_yValue = screenGeometry.top() - shadowWidth();
        t_endRect = QRect(t_xValue, t_yValue, t_width, t_height);
    }
    else if (d->m_enDriection & d->Left)
    {
        t_xValue = screenGeometry.left() - shadowWidth();
        t_yValue = this->y();
        t_endRect = QRect(t_xValue, t_yValue, t_width, t_height);
    }
    else if (d->m_enDriection & d->Right)
    {
        t_xValue = (screenGeometry.left() + screenGeometry.width()) - this->width() + shadowWidth() - PANEL_HIDEMARGIN;
        t_yValue = this->y();
        t_endRect = QRect(t_xValue, t_yValue, t_width, t_height);
    }
    else
    {
        t_endRect = this->rect();
    }

    t_animation->setEndValue(t_endRect);
    t_animation->start(QAbstractAnimation::DeleteWhenStopped);
}
