#include "abstractwidget.h"

#include <QPaintEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <qmath.h>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <windowsx.h>
#endif //Q_OS_WIN

#define WINDOW_MARGIN_SIZE 5             //边距宽度
#define WINDOW_MARGIN_WIDTH 10           //渐变背景宽度
#define WINDOW_CURSOR_DETECT_SIZE  8     //鼠标移动在边框移动时检测的范围

AbstractWidget::AbstractWidget(QWidget *parent):
    QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint |Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    contentWidget = new QWidget(this);

    QHBoxLayout * layout = new QHBoxLayout();
    layout->setContentsMargins(WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE);
    layout->setSpacing(0);
    layout->addWidget(contentWidget);
    this->setLayout(layout);
}

AbstractWidget::~AbstractWidget()
{

}

void AbstractWidget::setContentWidget(QWidget *child)
{
    if(!contentWidget->layout())
    {
        QHBoxLayout * layout = new QHBoxLayout;
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        contentWidget->setLayout(layout);
    }

    QHBoxLayout * layout = dynamic_cast<QHBoxLayout *>(contentWidget->layout());
    if(layout)
    {
        layout->addWidget(child);
    }
}

void AbstractWidget::paintEvent(QPaintEvent *)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(WINDOW_MARGIN_WIDTH, WINDOW_MARGIN_WIDTH, this->width()-WINDOW_MARGIN_WIDTH * 2, this->height()- WINDOW_MARGIN_WIDTH * 2);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i = 0; i< WINDOW_MARGIN_WIDTH; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(WINDOW_MARGIN_WIDTH-i, WINDOW_MARGIN_WIDTH-i, this->width()-(WINDOW_MARGIN_WIDTH-i)*2, this->height()-(WINDOW_MARGIN_WIDTH-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

bool AbstractWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef Q_OS_WIN
    //Yang 解决无边框下窗口的拖拽以及鼠标状态显示
    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_NCHITTEST)
    {
        if (isMaximized())
        {
            return false;
        }

        *result = 0;
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
            if (x >= winrect.left && x < winrect.left + WINDOW_CURSOR_DETECT_SIZE)
            {
                *result = HTLEFT;
            }
            //right border
            if (x < winrect.right && x >= winrect.right - WINDOW_CURSOR_DETECT_SIZE)
            {
                *result = HTRIGHT;
            }
        }
        if (resizeHeight)
        {
            //bottom border
            if (y < winrect.bottom && y >= winrect.bottom - WINDOW_CURSOR_DETECT_SIZE)
            {
                *result = HTBOTTOM;
            }
            //top border
            if (y >= winrect.top && y < winrect.top + WINDOW_CURSOR_DETECT_SIZE)
            {
                *result = HTTOP;
            }
        }
        if (resizeWidth && resizeHeight)
        {
            //bottom left corner
            if (x >= winrect.left && x < winrect.left + WINDOW_CURSOR_DETECT_SIZE &&
                y < winrect.bottom && y >= winrect.bottom - WINDOW_CURSOR_DETECT_SIZE)
            {
                *result = HTBOTTOMLEFT;
            }
            //bottom right corner
            if (x < winrect.right && x >= winrect.right - WINDOW_CURSOR_DETECT_SIZE &&
                y < winrect.bottom && y >= winrect.bottom - WINDOW_CURSOR_DETECT_SIZE)
            {
                *result = HTBOTTOMRIGHT;
            }
            //top left corner
            if (x >= winrect.left && x < winrect.left + WINDOW_CURSOR_DETECT_SIZE &&
                y >= winrect.top && y < winrect.top + WINDOW_CURSOR_DETECT_SIZE)
            {
                *result = HTTOPLEFT;
            }
            //top right corner
            if (x < winrect.right && x >= winrect.right - WINDOW_CURSOR_DETECT_SIZE &&
                y >= winrect.top && y < winrect.top + WINDOW_CURSOR_DETECT_SIZE)
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
