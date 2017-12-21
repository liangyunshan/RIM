#include "toolitem.h"

#include <QLabel>
#include <QGridLayout>
#include <QEvent>
#include <QEnterEvent>
#include <QStyle>
#include <QMenu>
#include <QDebug>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QMetaEnum>

#include "head.h"
#include "constants.h"
#include "util/rsingleton.h"
#include "util/imagemanager.h"
#include "Widgets/widget/riconlabel.h"

#define TOOL_ITEM_MAX_HEIGHT 56

class ToolItemPrivate : public GlobalData<ToolItem>
{
    Q_DECLARE_PUBLIC(ToolItem)

protected:
    ToolItemPrivate(ToolItem * q):q_ptr(q)
    {
        initWidget();
        checked = false;
        contenxMenu = NULL;
    }

    ToolItem * q_ptr;

    ToolPage * pagePtr;

    void initWidget();

    QWidget * contentWidget;
    RIconLabel * iconLabel;             //个人、群组、历史聊天的头像
    QLabel * nameLabel;                 //个人、群组、历史聊天的用户名
    QLabel * nickLabel;                 //个人昵称、群组成员数量
    QLabel * descLabel;                 //个人签名、群组和历史聊天的聊天记录信息
    QLabel * infoLabel;                 //群组和历史聊天的日期

    QMenu * contenxMenu;

    bool checked;                       //是否被选中
};

void ToolItemPrivate::initWidget()
{
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Tool_Item_ContentWidget");
    QGridLayout * gridLayout = new QGridLayout;

    QWidget * iconWidget = new QWidget;
    iconWidget->setFixedSize(50,50);
    QHBoxLayout * iconLayout = new QHBoxLayout;
    iconLayout->setContentsMargins(0,0,0,0);
    iconLayout->setSpacing(0);

    iconLabel = new RIconLabel();
    iconLabel->setTransparency(true);
    iconLabel->setEnterCursorChanged(false);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(QPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon()));
    iconLabel->setFixedSize(Constant::ICON_USER_SIZE,Constant::ICON_USER_SIZE);

    iconLayout->addStretch(1);
    iconLayout->addWidget(iconLabel);
    iconWidget->setLayout(iconLayout);

    QWidget * middleWidget = new QWidget(contentWidget);
    middleWidget->setFixedHeight(Constant::ICON_USER_SIZE);

    QVBoxLayout * middleLayout = new QVBoxLayout();
    middleLayout->setContentsMargins(0,0,0,0);
    middleLayout->setSpacing(1);

    QWidget * nameWidget = new QWidget(middleWidget);
    QHBoxLayout * nameLayout = new QHBoxLayout;
    nameLayout->setContentsMargins(0,0,0,0);
    nameLayout->setSpacing(0);

    nameLabel = new QLabel(nameWidget);
    nameLabel->setFixedHeight(20);
    nameLabel->setObjectName("Tool_Item_NameLabel");

    nickLabel = new QLabel(nameWidget);
    nickLabel->setFixedHeight(20);
    nickLabel->setObjectName("Tool_Item_NickLabel");

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nickLabel);
    nameLayout->addStretch(1);

    nameWidget->setLayout(nameLayout);

    descLabel = new QLabel(contentWidget);
    descLabel->setFixedHeight(20);
    descLabel->setObjectName("Tool_Item_DescLabel");

    middleLayout->addWidget(nameWidget);
    middleLayout->addWidget(descLabel);
    middleWidget->setLayout(middleLayout);

    infoLabel = new QLabel(contentWidget);
    infoLabel->setFixedWidth(25);

    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSpacing(3);
    gridLayout->setRowStretch(0,5);
    gridLayout->addWidget(iconWidget,1,0,1,1);
    gridLayout->addWidget(middleWidget,1,1,1,1);
    gridLayout->addWidget(infoLabel,1,2,1,1);
    gridLayout->setRowStretch(3,1);

    contentWidget->setLayout(gridLayout);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    contentWidget->installEventFilter(q_ptr);
}

ToolItem::ToolItem(ToolPage *page, QWidget *parent) :
    QWidget(parent),
    d_ptr(new ToolItemPrivate(this))
{
    d_ptr->pagePtr = page;
    setFixedHeight(TOOL_ITEM_MAX_HEIGHT);
}

void ToolItem::setIcon(QString icon)
{

}

QString ToolItem::icon()
{

}

void ToolItem::setName(QString name)
{
    MQ_D(ToolItem);
    d->nameLabel->setText(name);
}

void ToolItem::setNickName(QString name)
{
    MQ_D(ToolItem);
    d->nickLabel->setText(QString("(%1)").arg(name));
}

void ToolItem::setDescInfo(QString text)
{
    MQ_D(ToolItem);
    d->descLabel->setText(text);
}

void ToolItem::setGroupMemberCoung(int number)
{
    MQ_D(ToolItem);
    d->nickLabel->setText(QString("%1").arg(number));
}

bool ToolItem::eventFilter(QObject *watched, QEvent *event)
{
    MQ_D(ToolItem);
    if(watched == d->contentWidget)
    {
        if(event->type() == QEvent::Enter)
        {
            if(!d->checked)
            {
                setItemState(Mouse_Enter);
            }
        }
        else if(event->type() == QEvent::Leave)
        {
            if(!d->checked)
            {
                setItemState(Mouse_Leave);
            }
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent * e = dynamic_cast<QMouseEvent *>(event);
            if(e->button() == Qt::LeftButton || e->button() == Qt::RightButton)
            {
                setChecked(true);
                emit clearSelectionOthers(this);
            }
        }
        else if(event->type() == QEvent::ContextMenu)
        {
            if(d->contenxMenu)
            {
                d->contenxMenu->exec(QCursor::pos());
                return true;
            }
        }
        else if(event->type() == QEvent::MouseButtonDblClick)
        {
            emit itemDoubleClick(this);
        }
    }
    return QWidget::eventFilter(watched,event);
}

void ToolItem::setItemState(ToolItem::ItemState state)
{
    MQ_D(ToolItem);
#if QT_VERSION >= 0x050500
    QMetaEnum metaEnum = QMetaEnum::fromType<ItemState>();
    d->contentWidget->setProperty(metaEnum.name(),metaEnum.key(state));
#else
    switch( state)
    {
        case Mouse_Enter:
                            d->contentWidget->setProperty("ItemState","Mouse_Enter");
                            break;
        case Mouse_Leave:
                            d->contentWidget->setProperty("ItemState","Mouse_Leave");
                            break;
        case Mouse_Checked:
                            d->contentWidget->setProperty("ItemState","Mouse_Checked");
                            break;
    }
#endif
    style()->unpolish(d->contentWidget);
    style()->polish(d->contentWidget);
}

void ToolItem::setContentMenu(QMenu *contentMenu)
{
    MQ_D(ToolItem);
    d->contenxMenu = contentMenu;
}

bool ToolItem::isChecked()
{
    MQ_D(ToolItem);
    return d->checked;
}

void ToolItem::setChecked(bool flag)
{
    MQ_D(ToolItem);
    d->checked = flag;
    if(flag)
    {
        setItemState(Mouse_Checked);
    }
    else
    {
        setItemState(Mouse_Leave);
    }
}

