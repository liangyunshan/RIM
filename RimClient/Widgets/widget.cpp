#include "Widget.h"

#include <QPaintEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <qmath.h>
#include <QMessageBox>
#include <QApplication>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QStyle>
#include <QTimer>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <windowsx.h>
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")
#endif //Q_OS_WIN

#include "head.h"
#include "constants.h"
#include "datastruct.h"

class WidgetPrivate : public GlobalData<Widget>
{
    Q_DECLARE_PUBLIC(Widget)

protected:
    WidgetPrivate(Widget * q):q_ptr(q)
    {
        isMousePressed = false;
        stickTopHint = false;
        isShadowVisible = true;
        toolBar = NULL;

        contentWidget = new QWidget(q);
        contentWidget->setObjectName("AbstractWidget_ContentWidget");

        QHBoxLayout * layout = new QHBoxLayout();
        layout->setContentsMargins(WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE);
        layout->setSpacing(0);
        layout->addWidget(contentWidget);

        q_ptr->setLayout(layout);
    }

    Widget * q_ptr;

    QWidget * contentWidget;
    ToolBar * toolBar;

    QPoint mousePressPoint;
    bool isMousePressed;
    bool stickTopHint;
    bool isShadowVisible;
};

Widget::Widget(QWidget *parent):
    d_ptr(new WidgetPrivate(this)),
    QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint |Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
}

Widget::~Widget()
{

}

void Widget::setShadowWindow(bool flag)
{
    MQ_D(Widget);
    d->isShadowVisible = flag;
    if(flag)
    {
          QTimer::singleShot(25,this,SLOT(setLayoutMargin()));
    }
    else
    {
        dynamic_cast<QHBoxLayout *>(layout())->setContentsMargins(0,0,0,0);
    }
    update();
}

void Widget::setContentWidget(QWidget *child)
{
    MQ_D(Widget);
    if(!d->contentWidget->layout())
    {
        QVBoxLayout * layout = new QVBoxLayout;
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        d->contentWidget->setLayout(layout);
    }

    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());
    if(layout)
    {
        layout->addWidget(child);
    }
}

#define ABSTRACT_TOOL_BAR_HEGIHT 30
ToolBar * Widget::enableToolBar(bool flag)
{
    MQ_D(Widget);
    if(flag && !d->toolBar)
    {
        d->toolBar = new ToolBar(d->contentWidget);
        d->toolBar->setObjectName("Widget_ToolBarWidget");
        d->toolBar->setFixedHeight(ABSTRACT_TOOL_BAR_HEGIHT);

        connect(d->toolBar,SIGNAL(minimumWindow()),this,SLOT(showMinimized()));
        connect(d->toolBar,SIGNAL(maxWindow(bool)),this,SLOT(showMaximizedWindow(bool)));
        connect(d->toolBar,SIGNAL(closeWindow()),this,SLOT(close()));

        if(!d->contentWidget->layout())
        {
            QVBoxLayout * layout = new QVBoxLayout;
            layout->setContentsMargins(0,0,0,0);
            layout->setSpacing(0);
            d->contentWidget->setLayout(layout);
        }

        QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());
        if(layout)
        {
            layout->insertWidget(0,d->toolBar);
        }

        return d->toolBar;
    }
    else if(d->toolBar)
    {
        d->toolBar->setVisible(flag);
    }

    return NULL;
}

void Widget::paintEvent(QPaintEvent *)
{
    MQ_D(Widget);
    if(d->isShadowVisible)
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
}

bool Widget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MQ_D(Widget);
#ifdef Q_OS_WIN
    if(d->isShadowVisible)
    {
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
            if (action == this)
            {
                *result = HTCAPTION;
                return true;
            }
            //在鼠标进入中间区域后，将图标设置默认值
            setCursor(Qt::ArrowCursor);
        }
    }
#else
    QMessageBox::warning(this,tr("Warning"),tr("System don't support resize window!"),QMessageBox::Yes,QMessageBox::Yes);
#endif
    return false;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    MQ_D(Widget);
    d->isMousePressed = true;
    d->mousePressPoint = event->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    MQ_D(Widget);
    if(d->isMousePressed)
    {
        QPoint tempPos = pos() + event->pos() - d->mousePressPoint;
        move(tempPos);
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    MQ_D(Widget);

    if(pos().y() <= 0)
    {
        move(pos().x(),-WINDOW_MARGIN_SIZE);
    }

    if(pos().x() < 0)
    {
        move(0,pos().y());
    }

    if(pos().x() > qApp->desktop()->geometry().width()  - width() )
    {
        move(qApp->desktop()->geometry().width() - width(),pos().y());
    }

    d->isMousePressed = false;
}

void Widget::enterEvent(QEvent *)
{
    if(pos().y() <= 0)
    {
        move(pos().x(), -WINDOW_MARGIN_SIZE);
    }
}

void Widget::leaveEvent(QEvent *)
{

}

void Widget::repolish(QWidget *widget)
{
    style()->unpolish(widget);
    style()->polish(widget);
}

void Widget::setLayoutMargin()
{
    dynamic_cast<QHBoxLayout *>(layout())->setContentsMargins(WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE);
    update();
}

void Widget::showMaximizedWindow(bool flag)
{

}
