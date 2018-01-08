#include "rtoolbutton.h"

#include <QEvent>
#include <QToolTip>
#include <QHelpEvent>
#include <QMetaEnum>
#include <QStyle>

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
    setItemState(ToolItem::Mouse_Leave);
    connect(this,SIGNAL(toggled(bool)),this,SLOT(updateItemStyle(bool)));
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

void RToolButton::removeIcon()
{
    if(!icon().isNull())
    {
        setIcon(QIcon());
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
    else if(e->type() == QEvent::Enter)
    {
        if(!isChecked())
            setItemState(ToolItem::Mouse_Enter);
    }
    else if(e->type() == QEvent::Leave)
    {
        if(!isChecked())
            setItemState(ToolItem::Mouse_Leave);
    }
    return QWidget::event(e);
}

void RToolButton::updateItemStyle(bool flag)
{
    setItemState(flag?ToolItem::Mouse_Checked:ToolItem::Mouse_Leave);
}

void RToolButton::setItemState(ToolItem::ItemState state)
{
#if QT_VERSION >= 0x050500
    QMetaEnum metaEnum = QMetaEnum::fromType<ToolItem::ItemState>();
    setProperty(metaEnum.name(),metaEnum.key(state));
#else
    switch( state)
    {
        case Mouse_Enter:
                            setProperty("ItemState","Mouse_Enter");
                            break;
        case Mouse_Leave:
                            setProperty("ItemState","Mouse_Leave");
                            break;
        case Mouse_Checked:
                            setProperty("ItemState","Mouse_Checked");
                            break;
        default:
            break;
    }
#endif
    style()->unpolish(this);
    style()->polish(this);
}

