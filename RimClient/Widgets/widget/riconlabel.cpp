#include "riconlabel.h"

#include <QPainter>
#include <QColor>
#include <QTimer>

#include "head.h"
#include "datastruct.h"

class RIconLabelPrivate : public GlobalData<RIconLabel>
{
    Q_DECLARE_PUBLIC(RIconLabel)

private:
    RIconLabelPrivate(RIconLabel * q):
        q_ptr(q)
    {
        timer = NULL;
        corner = true;
        hoverDealyView = true;
        transparency = false;
        cursorChange = true;
        backgroundColor = Qt::white;
    }

    RIconLabel * q_ptr;

    bool corner;
    bool transparency;                      //透明度
    bool cursorChange;
    bool hoverDealyView;                    //延迟发送进入信号
    QColor backgroundColor;
    QTimer * timer;
};

RIconLabel::RIconLabel(QWidget *parent):
    d_ptr(new RIconLabelPrivate(this)),
    QLabel(parent)
{

}

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

void RIconLabel::setTransparency(bool flag)
{
    MQ_D(RIconLabel);
    d->transparency = flag;
    update();
}

void RIconLabel::setEnterCursorChanged(bool flag)
{
    MQ_D(RIconLabel);
    d->cursorChange = flag;
}

void RIconLabel::setHoverDelay(bool flag)
{
    MQ_D(RIconLabel);
    d->hoverDealyView = flag;
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
    emit mouseHover(false);
}

void RIconLabel::timeOut()
{
    emit mouseHover(true);
}
