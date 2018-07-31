#include "widget.h"

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
#ifdef Q_CC_MSVC
#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")
#endif
#endif //Q_OS_WIN

#include "head.h"
#include "constants.h"
#include "../protocol/datastruct.h"

class WidgetPrivate : public GlobalData<Widget>
{
    Q_DECLARE_PUBLIC(Widget)

protected:
    WidgetPrivate(Widget * q):q_ptr(q),isMousePressed(false),stickTopHint(false),
        isShadowVisible(true),windowMoveable(true),toolBar(nullptr),toolbarMoveable(false)
    {
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
    bool isShadowVisible;               //边框阴影是否可见
    bool windowMoveable;                //窗口整体是否可移动(默认为true)
    bool toolbarMoveable;               //工具栏是否可移动(默认为false，设置为true时，windowMoveable无效，此设置优先级较高)
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

/*!
 * @brief 设置是否开启窗口阴影
 * @param[in] flag 开启标识
 * @return 无
 */
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
        update();
    }
}

/*!
 * @brief 设置窗口是否可移动
 * @details 默认为true可移动，整个窗口都可以响应移动事件。
 * @param[in] flag 状态
 * @return 无
 */
void Widget::setWindowsMoveable(bool flag)
{
    MQ_D(Widget);
    d->windowMoveable = flag;
}

/*!
 * @brief 设置工具栏可移动，其它部位不可移动
 * @attention 此设置优先级比设置WindowsMoveable要高，两者最好只设置一个
 * @param[in] flag 设置移动标识
 * @return 是否插入成功
 */
void Widget::setToolBarMoveable(bool flag)
{
    MQ_D(Widget);
    d->toolbarMoveable = flag;
}

/*!
 * @brief 将窗口加入至主窗口区域
 * @param[in] child 待加入的窗口
 * @return 无
 */
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

/*!
 * @brief 是否需要开启工具栏,开启工具栏后可对工具栏进行动态的配置
 * @param[in] falg 控制标识
 * @return ToolBar * 创建的工具栏，可进行进一步操作
 */
ToolBar * Widget::enableToolBar(bool flag)
{
    MQ_D(Widget);
    if(flag && !d->toolBar)
    {
        d->toolBar = new ToolBar(d->contentWidget);
        d->toolBar->setObjectName("Widget_ToolBarWidget");
        d->toolBar->setFixedHeight(ABSTRACT_TOOL_BAR_HEGIHT);

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

/*!
 * @brief 是否开启默认的信号连接；若为true，则窗口工具栏点击后，不会发送信号；
 * @param[in] flag 状态
 * @return 无
 */
void Widget::enableDefaultSignalConection(bool flag)
{
    MQ_D(Widget);
    if(flag){
        connect(d->toolBar,SIGNAL(minimumWindow()),this,SLOT(showMinimized()));
        connect(d->toolBar,SIGNAL(maxWindow(bool)),this,SLOT(showMaximizedWindow(bool)));
        connect(d->toolBar,SIGNAL(closeWindow()),this,SLOT(close()));
    }else{
        connect(d->toolBar,SIGNAL(minimumWindow()),this,SIGNAL(minimumWindow()));
        connect(d->toolBar,SIGNAL(maxWindow(bool)),this,SIGNAL(maxWindow(bool)));
        connect(d->toolBar,SIGNAL(closeWindow()),this,SIGNAL(closeWindow()));
    }
}

/*!
 * @brief 绘图事件
 * @details 绘制窗口四周边框阴影，绘制时采用渐变的方式逐级向外降低透明度。每边绘制的宽度使用WINDOW_MARGIN_WIDTH变量的定义。
 * @see WINDOW_MARGIN_WIDTH
 * @param[in] event 事件句柄
 * @return 无
 */
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

/*!
 * @brief 操作系统原生事件
 * @details 调用Windows原生的事件，用于处理无边框窗口中鼠标移动至各个边框后调整至对应样式以及处理窗口缩放
 * @param[in] eventType 事件类型
 * @param[in] message 消息类型
 * @param[in] result 操作结果
 * @return 是否处理
 */
bool Widget::nativeEvent(const QByteArray &/*eventType*/, void *message, long *result)
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
    if(d->toolbarMoveable && d->toolBar && d->isMousePressed){
        if(d->toolBar->geometry().contains(event->pos())){
            QPoint tempPos = pos() + event->pos() - d->mousePressPoint;
            move(tempPos);
        }
    }else{
        if(d->windowMoveable && d->isMousePressed)
        {
            QPoint tempPos = pos() + event->pos() - d->mousePressPoint;
            move(tempPos);
        }
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    MQ_D(Widget);

    d->isMousePressed = false;
    if(pos().y() + shadowWidth() < 0)
    {
        move(pos().x(), -WINDOW_MARGIN_SIZE);
    }
    if(pos().x() + shadowWidth() < 0)
    {
        move(-WINDOW_MARGIN_SIZE, pos().y());
    }
    //防止右出界
    QDesktopWidget* desktopWidget = QApplication::desktop();
    int t_count = desktopWidget->screenCount();
    int t_width = 0;
    int t_height = desktopWidget->screenGeometry(pos()).height();
    for(int index = 0;index < t_count;index++)
    {
        t_width += desktopWidget->screenGeometry(index).width();
    }
    if(pos().x()+width() > t_width)
    {
        move(t_width-width()+WINDOW_MARGIN_SIZE, pos().y());
    }
    //防止下出界
    if(pos().y()+height() > t_height)
    {
        move(pos().x(), t_height-height()+WINDOW_MARGIN_SIZE);
    }
}

void Widget::enterEvent(QEvent *)
{

}

void Widget::leaveEvent(QEvent *)
{

}

/*!
 * @brief 对控件重新应用样式
 * @details 动态的改变控件的样式
 * @param[in] widget 待更新样式的控件
 * @return 无
 */
void Widget::repolish(QWidget *widget)
{
    if(!widget)
    {
        return;
    }
    style()->unpolish(widget);
    style()->polish(widget);
}

/*!
 * @brief 获取窗口阴影宽度
 * @details 当阴影可见时返回WINDOW_MARGIN_SIZE否则返回0
 * @param[in] 无
 * @return WINDOW_MARGIN_SIZE或者0
 */
int Widget::shadowWidth()
{
    MQ_D(Widget);
    int t_width = 0;
    if(d->isShadowVisible)
    {
        t_width = WINDOW_MARGIN_SIZE;
    }
    else
    {
        t_width= 0;
    }
    return t_width;
}

void Widget::setLayoutMargin()
{
    dynamic_cast<QHBoxLayout *>(layout())->setContentsMargins(WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE,WINDOW_MARGIN_SIZE);
    update();
}

void Widget::showMaximizedWindow(bool )
{

}
