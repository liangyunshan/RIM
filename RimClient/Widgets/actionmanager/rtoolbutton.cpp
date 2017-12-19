#include "rtoolbutton.h"

#include <QEvent>
#include <QToolTip>
#include <QHelpEvent>
#include <QDebug>

#include "constants.h"
#include "datastruct.h"
#include "head.h"

class RToolButtonPrivate : public GlobalData<RToolButton>
{
    Q_DECLARE_PUBLIC(RToolButton)

private:
    QString toolTipInfo;
};

RToolButton::RToolButton(QWidget *parent):
    d_ptr(new RToolButtonPrivate()),
    QToolButton(parent)
{

}

RToolButton::~RToolButton()
{

}

void RToolButton::setToolTip(const QString &toolTipInfo)
{
    MQ_D(RToolButton);
    d->toolTipInfo = toolTipInfo;
}

void RToolButton::setObjectName(QString objName)
{
    QToolButton::setObjectName(objName);

    if(icon().isNull())
    {
        setIcon(QIcon(QString(":/icon/resource/icon/%1.png").arg(objName)));
    }
}

bool RToolButton::event(QEvent *e)
{
    MQ_D(RToolButton);
    if(e->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
        QToolTip::showText(helpEvent->globalPos(), d->toolTipInfo);

        return true;
    }
    return QWidget::event(e);
}

