#include "rlabel.h"

#include <QPainter>
#include <QColor>
#include <QTimer>

#include "head.h"
#include "datastruct.h"

RTextLabel::RTextLabel(QWidget * parent):QLabel(parent)
{

}

void RTextLabel::enterEvent(QEvent *)
{
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("color:red");
}

void RTextLabel::leaveEvent(QEvent *)
{
    setCursor(Qt::ArrowCursor);
    setStyleSheet("color:black");
}

void RTextLabel::mouseReleaseEvent(QMouseEvent *)
{
    emit mousePress();
}


class RIconLabelPrivate : public GlobalData<RIconLabel>
{
    Q_DECLARE_PUBLIC(RIconLabel)

private:
    RIconLabelPrivate(RIconLabel * q):
        q_ptr(q)
    {
        timer = NULL;
        corner = true;
        resizePixmap = false;
        hoverDealyView = true;
        transparency = false;
        cursorChange = true;
        enterHighlight = false;
        hightLight = false;
        backgroundColor = Qt::white;
        enterHightPen.setColor(Qt::red);
        enterHightPen.setWidth(2);
    }

    RIconLabel * q_ptr;

    bool corner;
    bool transparency;                      //透明度
    bool cursorChange;
    bool hoverDealyView;                    //延迟发送进入信号
    bool resizePixmap;                      //控件尺寸参照图片尺寸
    bool enterHighlight;                    //鼠标进入高亮显示
    bool hightLight;
    QColor backgroundColor;
    QPen enterHightPen;                     //鼠标进入后
    QString fileName;                       //文件名
    QTimer * timer;
};

RIconLabel::RIconLabel(QWidget *parent):
    d_ptr(new RIconLabelPrivate(this)),
    QLabel(parent)
{

}

/*!
 * @brief 是否开启圆角显示
 * @details 开启圆角时，会以宽度和高度中最小作为直径
 * @param[in] isCorner 默认为true
 * @return 无
 */
void RIconLabel::setCorner(bool isCorner)
{
    MQ_D(RIconLabel);
    d->corner = isCorner;
}

void RIconLabel::setBackroundColor(QColor color)
{
    MQ_D(RIconLabel);
    d->backgroundColor = color;
}

/*!
 * @brief 设置背景透明
 * @details 在corner为true时，设置其背景是否透明；若为false，背景颜色可通过setBackgroundColor设置
 * @param[in] flag 默认为true
 * @return 无
 */
void RIconLabel::setTransparency(bool flag)
{
    MQ_D(RIconLabel);
    d->transparency = flag;
    update();
}

/*!
 * @brief 设置鼠标进入控件，是否改变鼠标的样式
 * @details 设置为true时，鼠标进入控件区域，样式修改为Qt::PointingHandCursor,离开时恢复为Qt::ArrowCursor
 * @param[in] flag 状态标识
 * @return 无
 */
void RIconLabel::setEnterCursorChanged(bool flag)
{
    MQ_D(RIconLabel);
    d->cursorChange = flag;
}

/*!
 * @brief 设置是否延迟发送鼠标进入信号
 * @details 设置鼠标进入控件之后，是否开启延迟发送进入信号
 * @param[in] toolButton 待插入的工具按钮
 * @return 无
 */
void RIconLabel::setHoverDelay(bool flag)
{
    MQ_D(RIconLabel);
    d->hoverDealyView = flag;
}

/*!
 * @brief 开启鼠标进入后高亮边框显示
 * @param[in] flag 默认为false
 * @return 无
 */
void RIconLabel::setEnterHighlight(bool flag)
{
    MQ_D(RIconLabel);
    d->enterHighlight = flag;
}

/*!
 * @brief 设置鼠标进入边框颜色
 * @details 在设置enterHighlight为true时有效
 * @param[in] color 颜色
 * @return 无
 */
void RIconLabel::setEnterHighlightColor(QColor color)
{
    MQ_D(RIconLabel);
    d->enterHightPen.setColor(color);
}

void RIconLabel::setPixmap(const QString &fileName)
{
    MQ_D(RIconLabel);

    if(fileName.size() == 0)
    {
        return;
    }

    d->fileName = fileName;
    if(d->resizePixmap)
    {
        setFixedSize(QPixmap(fileName).size());
    }
    QLabel::setPixmap(QPixmap(fileName));
}

QFileInfo RIconLabel::getPixmapFileInfo()
{
    MQ_D(RIconLabel);
    return QFileInfo(d->fileName);
}

/*!
 * @brief 控件尺寸参考图片
 * @param[in] flag
 * @return 无
 */
void RIconLabel::resizeByPixmap(bool flag)
{
    MQ_D(RIconLabel);
    d->resizePixmap = flag;
    if(pixmap())
    {
        setFixedSize(pixmap()->size());
    }
}

void RIconLabel::paintEvent(QPaintEvent *)
{
    MQ_D(RIconLabel);
    if(d->corner)
    {
        QPainter painter(this);

        if(d->transparency)
        {
            painter.fillRect(rect(),QBrush(QColor(0,0,0,0)));
        }
        else
        {
            painter.fillRect(rect(),QBrush(d->backgroundColor));
        }

        if(pixmap())
        {
            painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            QPainterPath path;
            int round = qMin(width(), height());
            path.addEllipse(0, 0, round, round);
            painter.setClipPath(path);
            painter.drawPixmap(0, 0, width(), height(), *pixmap());
        }
        else
        {
            painter.fillRect(rect(),d->backgroundColor);
        }

        if(d->hightLight)
        {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(d->enterHightPen);
            int round = qMin(width(), height());
            painter.drawEllipse(0, 0, round, round);
        }
    }
    else
    {
        QPainter painter(this);
        painter.drawPixmap(0, 0, width(), height(), *pixmap());
        if(d->hightLight)
        {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(d->enterHightPen);
            int penWidth = d->enterHightPen.width();
            painter.drawRect(rect().adjusted(penWidth,penWidth,-penWidth,-penWidth));
        }
    }
}

void RIconLabel::enterEvent(QEvent *)
{
    MQ_D(RIconLabel);
    if(d->cursorChange)
    {
        setCursor(Qt::PointingHandCursor);
    }

    if(d->hoverDealyView)
    {
        if(!d->timer)
        {
            d->timer = new QTimer;
            d->timer->setSingleShot(true);
            d->timer->setInterval(450);
            connect(d->timer,SIGNAL(timeout()),this,SLOT(timeOut()));
        }
        d->timer->start();
    }
    else
    {
        timeOut();
    }

    if(d->enterHighlight)
    {
        d->hightLight = true;
        update();
    }
}

void RIconLabel::leaveEvent(QEvent *)
{
    MQ_D(RIconLabel);
    setCursor(Qt::ArrowCursor);

    if(d->hoverDealyView)
    {
        if(d->timer)
        {
            if(d->timer->isActive())
            {
                d->timer->stop();
            }
        }
    }
    if(d->enterHighlight)
    {
        d->hightLight = false;
        update();
    }
    emit mouseHover(false);
}

void RIconLabel::mousePressEvent(QMouseEvent *)
{
    emit mousePressed();
}

void RIconLabel::mouseReleaseEvent(QMouseEvent *)
{
    emit mouseReleased();
}

void RIconLabel::timeOut()
{
    emit mouseHover(true);
}
