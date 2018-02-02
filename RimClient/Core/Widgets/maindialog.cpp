#include "maindialog.h"

#include <QDesktopWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QHBoxLayout>
#include <QApplication>
#include <QMessageBox>
#include <QToolButton>

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
#include "rsingleton.h"
#include "actionmanager/shortcutsettings.h"
#include "editpersoninfowindow.h"
#include "messdiapatch.h"
#include "widget/rmessagebox.h"
#include "user/userclient.h"
#include "media/mediaplayer.h"

#include "abstractchatwidget.h"
#include "itemhoverinfo.h"

#include "sql/databasemanager.h"
#include "screenshot.h"

#define PANEL_MARGIN 20

class MainDialogPrivate : public GlobalData<MainDialog>
{
    Q_DECLARE_PUBLIC(MainDialog)

    MainDialogPrivate(MainDialog *q):q_ptr(q)
    {
        editWindow = NULL;
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

    MainDialog * q_ptr;
};

MainDialog * MainDialog::dialog = NULL;

MainDialog::MainDialog(QWidget *parent) :
    d_ptr(new MainDialogPrivate(this)),
    Widget(parent)
{
    setMinimumSize(Constant::MAIN_PANEL_MIN_WIDTH,Constant::MAIN_PANEL_MIN_HEIGHT);
    setMaximumWidth(Constant::MAIN_PANEL_MAX_WIDTH);
    setMaximumHeight(qApp->desktop()->screen()->height());

    dialog = this;
    RSingleton<Subject>::instance()->attach(this);
    ScreenShot::instance();
    initSqlDatabase();
    initWidget();

    connect(MessDiapatch::instance(),SIGNAL(recvFriendList(FriendListResponse*)),this,SLOT(updateFriendList(FriendListResponse*)));
    connect(MessDiapatch::instance(),SIGNAL(recvGroupingOperate(GroupingResponse)),this,SLOT(recvGroupingOperate(GroupingResponse)));
    connect(MessDiapatch::instance(),SIGNAL(errorGroupingOperate(OperateGrouping)),this,SLOT(errorGroupingOperate(OperateGrouping)));
}

MainDialog::~MainDialog()
{
    MQ_D(MainDialog);
    RSingleton<Subject>::instance()->detach(this);
    if(d->editWindow)
    {
        delete d->editWindow;
    }

    RSingleton<UserManager>::instance()->closeAllClientWindow();

    RSingleton<ShortcutSettings>::instance()->save();
    if(p_dbManager)
    {
        delete p_dbManager;
        p_dbManager = NULL;
    }
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
                                 makeWindowFront(RUtil::globalSettings()->value(Constant::SETTING_TOPHINT).toBool());
                           }
                            break;
        default:break;
    }
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
    if(RUtil::globalSettings()->value(Constant::SETTING_EXIT_SYSTEM).toBool())
    {
        this->close();
    }
    else
    {
        this->hide();
    }
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

    RUtil::globalSettings()->setValue(Constant::SETTING_TOPHINT,flag);

    show();
}

void MainDialog::showChatWindow(ToolItem * item)
{
    MQ_D(MainDialog);

    UserClient * client = RSingleton<UserManager>::instance()->client(item);
    if(client->chatWidget)
    {
        client->chatWidget->show();
    }
    else
    {
        AbstractChatWidget * widget = new AbstractChatWidget();
        widget->setUserInfo(client->simpleUserInfo);
        client->chatWidget = widget;
        widget->show();
    }
}

void MainDialog::showHoverItem(bool flag, ToolItem * item)
{
    MQ_D(MainDialog);
    if(flag)
    {
        ItemHoverInfo * info = new ItemHoverInfo;
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

void MainDialog::updateFriendList(FriendListResponse *friendList)
{
    QList<RGroupData *>::iterator iter = G_FriendList.begin();
    while(iter != G_FriendList.end())
    {
        delete (*iter);
        iter = G_FriendList.erase(iter);
    }
    G_FriendList.clear();

    QList<RGroupData *>::iterator fiter = friendList->groups.begin();
    while(fiter != friendList->groups.end())
    {
        RGroupData * recvData = (*fiter);
        RGroupData * data = new RGroupData;
        data->groupId = recvData->groupId;
        data->groupName = recvData->groupName;
        data->isDefault = recvData->isDefault;
        data->users = recvData->users;

        G_FriendList.append(data);

        fiter = friendList->groups.erase(fiter);
    }
    friendList->groups.clear();
    delete friendList;

    RSingleton<Subject>::instance()->notify(MESS_FRIENDLIST_UPDATE);
}

//TODO 根据服务器返回的信息更新本地分组信息
void MainDialog::recvGroupingOperate(GroupingResponse response)
{

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

    readSettings();

    d->toolBar = new ToolBar(d->MainPanel);
    d->toolBar->setToolFlags(ToolBar::TOOL_ICON|ToolBar::TOOL_MIN|ToolBar::TOOL_CLOSE|ToolBar::TOOL_SPACER);
    connect(d->toolBar,SIGNAL(minimumWindow()),this,SLOT(showMinimized()));
    connect(d->toolBar,SIGNAL(closeWindow()),this,SLOT(closeWindow()));

    d->toolBar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM,ImageManager::ICON_16));

    RToolButton * frontButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_FRONT,this,SLOT(makeWindowFront(bool)),true);

    d->toolBar->insertToolButton(frontButton,Constant::TOOL_MIN);

    makeWindowFront(RUtil::globalSettings()->value(Constant::SETTING_TOPHINT).toBool());

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

void MainDialog::readSettings()
{
    QSettings * settings = RUtil::globalSettings();

    if(!settings->value(Constant::SETTING_X).isValid() || !settings->value(Constant::SETTING_Y).isValid()
            ||!settings->value(Constant::SETTING_WIDTH).isValid() ||!settings->value(Constant::SETTING_HEIGHT).isValid())
    {
        QRect rect = qApp->desktop()->screen()->geometry();

        int tmpWidth = Constant::MAIN_PANEL_MIN_WIDTH * SCALE_ZOOMIN_FACTOR;
        int tmpHeight = rect.height() * SCALE_ZOOMOUT_FACTOR;


        RUtil::globalSettings()->setValue(Constant::SETTING_X,rect.width() - tmpWidth - PANEL_MARGIN);
        RUtil::globalSettings()->setValue(Constant::SETTING_Y,PANEL_MARGIN);
        RUtil::globalSettings()->setValue(Constant::SETTING_WIDTH,tmpWidth);
        RUtil::globalSettings()->setValue(Constant::SETTING_HEIGHT,tmpHeight);
    }

    int x = RUtil::globalSettings()->value(Constant::SETTING_X).toInt();
    int y = RUtil::globalSettings()->value(Constant::SETTING_Y).toInt();
    int w = RUtil::globalSettings()->value(Constant::SETTING_WIDTH).toInt();
    int h = RUtil::globalSettings()->value(Constant::SETTING_HEIGHT).toInt();

    this->setGeometry(x,y,w,h);
}

void MainDialog::writeSettings()
{
    QRect rect = this->geometry();
    RUtil::globalSettings()->setValue(Constant::SETTING_X,rect.x());
    RUtil::globalSettings()->setValue(Constant::SETTING_Y,rect.y());
    RUtil::globalSettings()->setValue(Constant::SETTING_WIDTH,rect.width());
    RUtil::globalSettings()->setValue(Constant::SETTING_HEIGHT,rect.height());
}

void MainDialog::initSqlDatabase()
{
    p_dbManager = new DatabaseManager();
    p_dbManager->setConnectInfo("localhost","./rimclient.db","root","rengu123456");
    p_dbManager->setDatabaseType("QSQLITE");
    p_dbManager->newDatabase("sqlite1234");
}
