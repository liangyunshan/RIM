#include "slidebar.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QPainter>

#include "head.h"
#include "constants.h"
#include "datastruct.h"

class SlideBarPrivate : public GlobalData<SlideBar>
{
    Q_DECLARE_PUBLIC(SlideBar)

private:
    SlideBarPrivate(SlideBar * q):
        q_ptr(q)
    {
        mouseEnter = false;
        showSideState = true;

        contentWidget = new QWidget(q_ptr);

        QVBoxLayout * layout = new QVBoxLayout;
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);

        contentWidget->setLayout(layout);

        QHBoxLayout * mainLayout = new QHBoxLayout();
        mainLayout->setContentsMargins(0,0,0,0);
        mainLayout->setSpacing(0);

        mainLayout->addWidget(contentWidget);
        q_ptr->setLayout(mainLayout);
    }
    SlideBar * q_ptr;

    QWidget * contentWidget;

    bool mouseEnter;
    bool showSideState;
};

SlideBar::SlideBar(QWidget *parent):
    d_ptr(new SlideBarPrivate(this)),
    QWidget(parent)
{
    d_ptr->contentWidget->installEventFilter(this);
}

SlideBar::~SlideBar()
{

}

void SlideBar::setState(bool flag)
{
    MQ_D(SlideBar);
    d->showSideState = flag;
}

bool SlideBar::eventFilter(QObject *watched, QEvent *event)
{
    MQ_D(SlideBar);
    if(watched == d->contentWidget)
    {
        if(event->type() == QEvent::Enter)
        {
            d->mouseEnter = true;
            setCursor(Qt::PointingHandCursor);
            if(d->showSideState)
            {
                setToolTip(tr("Hide side widget"));
            }
            else
            {
                setToolTip(tr("Show side widget"));
            }
            update();
        }
        else if(event->type() == QEvent::Leave)
        {
            d->mouseEnter = false;
            setCursor(Qt::ArrowCursor);
            update();
        }
        else if(event->type() == QEvent::MouseButtonPress)
        {
            d->showSideState = !d->showSideState;
            emit slideStateChanged(d->showSideState);
        }
        else if(event->type() == QEvent::Paint)
        {
            if(d->mouseEnter)
            {
                QPainter painter(d->contentWidget);

                QPixmap pixmap;
                if(d->showSideState)
                {
                    pixmap.load(":/background/resource/background/slide_show.png");
                }
                else
                {
                    pixmap.load(":/background/resource/background/slide_hide.png");
                }

                QSize size = d->contentWidget->size();
                painter.drawPixmap(0,(size.height() - pixmap.height())/2,pixmap);
            }

            return true;
        }
    }
    return QWidget::eventFilter(watched,event);
}
