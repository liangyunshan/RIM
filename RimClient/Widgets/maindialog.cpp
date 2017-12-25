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
#include "toolbox/toolitem.h"

#include "abstractchatwidget.h"

#define PANEL_MARGIN 20

class MainDialogPrivate : public GlobalData<MainDialog>
{
    Q_DECLARE_PUBLIC(MainDialog)

    MainDialogPrivate(MainDialog *q):q_ptr(q)
    {

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

    QMap<ToolItem * ,AbstractChatWidget*> chatWidgets;

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

    initWidget();
}

MainDialog::~MainDialog()
{

}

MainDialog *MainDialog::instance()
{
    return dialog;
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

void MainDialog::showChatWindow(ToolItem * item)
{
    MQ_D(MainDialog);
    AbstractChatWidget * widget = NULL;
    if(d->chatWidgets.contains(item))
    {
        widget = d->chatWidgets.value(item);
    }
    else
    {
       widget = new AbstractChatWidget();

       d->chatWidgets.insert(item,widget);
    }

    widget->show();
}

#include <QToolButton>

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

    d->toolBar = new ToolBar(false,d->MainPanel);

    RToolButton * frontButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_FRONT,this,SLOT(makeWindowFront(bool)),true);

    RToolButton * minSize = ActionManager::instance()->createToolButton(Id(Constant::TOOL_MIN),this,SLOT(showMinimized()));
    minSize->setToolTip(tr("Min"));

    RToolButton * closeButt = ActionManager::instance()->createToolButton(Id(Constant::TOOL_CLOSE),this,SLOT(closeWindow()));
    closeButt->setToolTip(tr("Close"));

    d->toolBar->addStretch(1);
    d->toolBar->appendToolButton(frontButton);
    d->toolBar->appendToolButton(minSize);
    d->toolBar->appendToolButton(closeButt);

    makeWindowFront(RUtil::globalSettings()->value("Main/topHint").toBool());

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
