#include "maindialog.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QHBoxLayout>
#include <QApplication>
#include <QToolButton>

#include "systemtrayicon.h"
#include "Util/rutil.h"
#include "datastruct.h"
#include "constants.h"
#include "head.h"
#include "toolbar.h"
#include "panelbottomtoolbar.h"
#include "panelcontentarea.h"
#include "paneltoparea.h"
#include "actionmanager/actionmanager.h"

#define PANEL_MARGIN 20

class MainDialogPrivate : public GlobalData<MainDialog>
{
    Q_DECLARE_PUBLIC(MainDialog)

    MainDialogPrivate()
    {
        isMousePressed = false;
    }

private:
    QPoint mousePressPoint;
    bool isMousePressed;
};

MainDialog::MainDialog(QWidget *parent) :
    d_ptr(new MainDialogPrivate()),
    AbstractWidget(parent)
{
    setMinimumSize(Constant::MAIN_PANEL_MIN_WIDTH,Constant::MAIN_PANEL_MIN_HEIGHT);
    setMaximumWidth(Constant::MAIN_PANEL_MAX_WIDTH);
    setMaximumHeight(qApp->desktop()->screen()->height());

    initWidget();
}

MainDialog::~MainDialog()
{

}

void MainDialog::mousePressEvent(QMouseEvent *event)
{
    MQ_D(MainDialog);
    d->isMousePressed = true;
    d->mousePressPoint = event->pos();
}

void MainDialog::mouseMoveEvent(QMouseEvent *event)
{
    MQ_D(MainDialog);
    if(d->isMousePressed)
    {
        QPoint tempPos = event->pos() - d->mousePressPoint;
        move(pos() + tempPos);
    }
}

void MainDialog::mouseReleaseEvent(QMouseEvent *event)
{
    MQ_D(MainDialog);
    d->isMousePressed = false;
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
    QLayout * lay = layout();
    int right = 0;
    int left  = 0;
    if(lay)
    {
        QMargins margins = lay->contentsMargins();
        right = margins.right();
        left = margins.left();
    }
    toolBar->setGeometry(left,0,this->width() - right * 3,Constant::TOOL_HEIGHT);
}

void MainDialog::closeWindow()
{
    this->close();
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

    RUtil::globalSettings()->setValue("Main/topHint",flag);

    show();
}

#include <QToolButton>

void MainDialog::initWidget()
{
    MainPanel = new QWidget(this);
    MainPanel->setObjectName("MainPanel");

    setContentWidget(MainPanel);

    TopBar = new QWidget(this);
    TopBar->setFixedHeight(150);
    TopBar->setObjectName("TopBar");

    Conent = new QWidget(this);
    Conent->setObjectName("Conent");

    ToolBarWidget = new QWidget(this);
    ToolBarWidget->setObjectName("ToolBarWidget");

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(TopBar);
    mainLayout->addWidget(Conent);
    mainLayout->addWidget(ToolBarWidget);

    MainPanel->setLayout(mainLayout);

    connect(SystemTrayIcon::instance(),SIGNAL(quitApp()),this,SLOT(closeWindow()));
    connect(SystemTrayIcon::instance(),SIGNAL(showMainPanel()),this,SLOT(showNormal()));

    readSettings();

    toolBar = new ToolBar(MainPanel);

    RToolButton * frontButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_FRONT,this,SLOT(makeWindowFront(bool)),true);
    toolBar->insertToolButton(frontButton,Constant::TOOL_MIN);

    makeWindowFront(RUtil::globalSettings()->value("Main/topHint").toBool());

    connect(toolBar,SIGNAL(minimumWindow()),this,SLOT(showMinimized()));
    connect(toolBar,SIGNAL(closeWindow()),this,SLOT(closeWindow()));

    panelTopArea = new PanelTopArea(TopBar);
    QHBoxLayout * topAreaLayout = new QHBoxLayout();
    topAreaLayout->setContentsMargins(0,0,0,0);
    topAreaLayout->setSpacing(0);
    topAreaLayout->addWidget(panelTopArea);
    TopBar->setLayout(topAreaLayout);

    //中部内容区
    panelContentArea = new PanelContentArea(Conent);
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(panelContentArea);
    Conent->setLayout(contentLayout);

    //底部工具栏
    bottomToolBar = new PanelBottomToolBar(ToolBarWidget);
    QHBoxLayout * bottomToolLayout = new QHBoxLayout();
    bottomToolLayout->setContentsMargins(0,0,0,0);
    bottomToolLayout->setSpacing(0);
    bottomToolLayout->addWidget(bottomToolBar);
    ToolBarWidget->setLayout(bottomToolLayout);

    QTimer::singleShot(0,this,SLOT(updateWidgetGeometry()));
}

#define SCALE_ZOOMIN_FACTOR 1.2
#define SCALE_ZOOMOUT_FACTOR 0.75

void MainDialog::readSettings()
{
    QSettings * settings = RUtil::globalSettings();

    if(!settings->value("Main/x").isValid() || !settings->value("Main/y").isValid()
            ||!settings->value("Main/width").isValid() ||!settings->value("Main/height").isValid())
    {
        QRect rect = qApp->desktop()->screen()->geometry();

        int tmpWidth = Constant::MAIN_PANEL_MIN_WIDTH * SCALE_ZOOMIN_FACTOR;
        int tmpHeight = rect.height() * SCALE_ZOOMOUT_FACTOR;


        RUtil::globalSettings()->setValue("Main/x",rect.width() - tmpWidth - PANEL_MARGIN);
        RUtil::globalSettings()->setValue("Main/y",PANEL_MARGIN);
        RUtil::globalSettings()->setValue("Main/width",tmpWidth);
        RUtil::globalSettings()->setValue("Main/height",tmpHeight);
    }

    int x = RUtil::globalSettings()->value("Main/x").toInt();
    int y = RUtil::globalSettings()->value("Main/y").toInt();
    int w = RUtil::globalSettings()->value("Main/width").toInt();
    int h = RUtil::globalSettings()->value("Main/height").toInt();

    this->setGeometry(x,y,w,h);
}

void MainDialog::writeSettings()
{
    QRect rect = this->geometry();
    RUtil::globalSettings()->setValue("Main/x",rect.x());
    RUtil::globalSettings()->setValue("Main/y",rect.y());
    RUtil::globalSettings()->setValue("Main/width",rect.width());
    RUtil::globalSettings()->setValue("Main/height",rect.height());
}
