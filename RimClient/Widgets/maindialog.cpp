#include "maindialog.h"
#include "ui_maindialog.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QTimer>
#include <QMessageBox>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <windowsx.h>
#endif //Q_OS_WIN

#include "systemtrayicon.h"
#include "Util/rutil.h"
#include "datastruct.h"
#include "constants.h"
#include "head.h"
#include "toolbar.h"

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
    QWidget(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint |Qt::Tool);
    this->setMinimumSize(Constant::MAIN_PANEL_MIN_WIDTH,Constant::MAIN_PANEL_MIN_HEIGHT);
    this->setMaximumWidth(Constant::MAIN_PANEL_MAX_WIDTH);
    this->setMaximumHeight(qApp->desktop()->screen()->height());

    initWidget();
}

MainDialog::~MainDialog()
{
    delete ui;
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

bool MainDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_NCHITTEST)
    {
        if (isMaximized())
        {
            return false;
        }

        *result = 0;
        const LONG borderWidth = 8;
        RECT winrect;
        GetWindowRect(reinterpret_cast<HWND>(winId()), &winrect);

        // must be short to correctly work with multiple monitors (negative coordinates)
        short x = msg->lParam & 0x0000FFFF;
        short y = (msg->lParam & 0xFFFF0000) >> 16;

        bool resizeWidth = minimumWidth() != maximumWidth();
        bool resizeHeight = minimumHeight() != maximumHeight();
        if (resizeWidth)
        {
            //left border
            if (x >= winrect.left && x < winrect.left + borderWidth)
            {
                *result = HTLEFT;
            }
            //right border
            if (x < winrect.right && x >= winrect.right - borderWidth)
            {
                *result = HTRIGHT;
            }
        }
        if (resizeHeight)
        {
            //bottom border
            if (y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOM;
            }
            //top border
            if (y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOP;
            }
        }
        if (resizeWidth && resizeHeight)
        {
            //bottom left corner
            if (x >= winrect.left && x < winrect.left + borderWidth &&
                y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOMLEFT;
            }
            //bottom right corner
            if (x < winrect.right && x >= winrect.right - borderWidth &&
                y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOMRIGHT;
            }
            //top left corner
            if (x >= winrect.left && x < winrect.left + borderWidth &&
                y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOPLEFT;
            }
            //top right corner
            if (x < winrect.right && x >= winrect.right - borderWidth &&
                y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOPRIGHT;
            }
        }

        if (*result != 0)
            return true;

        QWidget *action = QApplication::widgetAt(QCursor::pos());
        if (action == this){
            *result = HTCAPTION;
            return true;
        }
        //在鼠标进入中间区域后，将图标设置默认值
        setCursor(Qt::ArrowCursor);
    }
#else
    QMessageBox::warning(this,tr("Warning"),tr("System don't support resize window!"),QMessageBox::Yes,QMessageBox::Yes);
#endif
    return false;
}

void MainDialog::updateWidgetGeometry()
{
    toolBar->setGeometry(0,0,this->width(),Constant::TOOL_HEIGHT);
}

void MainDialog::closeWindow()
{
    this->close();
}

#include <QToolButton>

void MainDialog::initWidget()
{
    connect(SystemTrayIcon::instance(),SIGNAL(quitApp()),this,SLOT(closeWindow()));
    connect(SystemTrayIcon::instance(),SIGNAL(showMainPanel()),this,SLOT(showNormal()));

    readSettings();

    toolBar = new ToolBar(this);
    connect(toolBar,SIGNAL(minimumWindow()),this,SLOT(showMinimized()));
    connect(toolBar,SIGNAL(closeWindow()),this,SLOT(closeWindow()));

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
