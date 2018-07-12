#include "toolitem.h"

#include <QLabel>
#include <QGridLayout>
#include <QEvent>
#include <QEnterEvent>
#include <QStyle>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QMetaEnum>
#include <QPainter>
#include <QImage>

#include "head.h"
#include "constants.h"
#include "util/rsingleton.h"
#include "Util/imagemanager.h"
#include "Widgets/widget/rlabel.h"
#include "Util/rutil.h"
#include "Widgets/onlinestate.h"

#define TOOL_ITEM_MAX_HEIGHT 56
#define TOOL_ITEM_INFOLABEL_WIDTH 25

class ToolItemPrivate : public GlobalData<ToolItem>
{
    Q_DECLARE_PUBLIC(ToolItem)

private:
    ToolItemPrivate(ToolItem * q):q_ptr(q),contenxMenu(nullptr),notifyCount(0),checked(false),topView(false){
        initWidget();
    }

    void updateNotifyInfoCount()
    {
        infoLabel->setAlignment(Qt::AlignCenter);
        infoLabel->setText(QString::number(++notifyCount));
    }

    ToolItem * q_ptr;

    ToolPage * pagePtr;

    void initWidget();

    QWidget * contentWidget;
    RIconLabel * iconLabel;             //个人、群组、历史聊天的头像
    QLabel * nameLabel;                 //个人、群组、历史聊天的用户名
    QLabel * nickLabel;                 //个人昵称、群组成员数量
    QLabel * descLabel;                 //个人签名、群组和历史聊天的聊天记录信息
    QLabel * infoLabel;                 //作为通知时的条数信息
    QLabel * timeLabel;                 //群组和历史聊天的日期
    QLabel * onLineStateLabel;          //联系人在线状态、群消息状态(屏蔽等).
    QLabel * topViewLabel;              //置顶显示状态，在topView为true时使用paint事件绘制状态效果
    QString originDescInfo;             //保存原始的显示信息

    int notifyCount;                    //作为通知消息时，显示通知消息的数量
    bool topView;                       //置顶显示，默认为false;置顶时提供角标显示其状态

    OnlineStatus onlineStatus;          //在线状态

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
    iconLabel->setPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon());
    iconLabel->setFixedSize(Constant::ICON_USER_SIZE,Constant::ICON_USER_SIZE);
    QObject::connect(iconLabel,SIGNAL(mouseHover(bool)),q_ptr,SLOT(cursorHoverIcon(bool)));

    iconLayout->addStretch(1);
    iconLayout->addWidget(iconLabel);
    iconWidget->setLayout(iconLayout);

    int tmpWidth = 18;
    onLineStateLabel = new QLabel(iconWidget);
    onLineStateLabel->setFixedSize(tmpWidth,tmpWidth);
    onLineStateLabel->move(iconWidget->width() - tmpWidth,iconWidget->height() - tmpWidth);

    QWidget * middleWidget = new QWidget(contentWidget);
    middleWidget->setFixedHeight(Constant::ICON_USER_SIZE);

    QVBoxLayout * middleLayout = new QVBoxLayout();
    middleLayout->setContentsMargins(5,0,0,0);
    middleLayout->setSpacing(1);

    QWidget * nameWidget = new QWidget(middleWidget);
//    nameWidget->setStyleSheet("background-color:red;");
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

    QMetaEnum metaEnum = QMetaEnum::fromType<ToolItem::ItemInfoLabelType>();

    QWidget * infoWidget = new QWidget();

    timeLabel = new QLabel(nameWidget);
    timeLabel->setFixedWidth(40);

    infoLabel = new QLabel(contentWidget);
    infoLabel->setObjectName("Tool_Item_InfoLabel");
    infoLabel->setProperty(metaEnum.name(),metaEnum.key(ToolItem::ItemTextInfo));
    infoLabel->setMinimumWidth(TOOL_ITEM_INFOLABEL_WIDTH);

    QVBoxLayout * infoLayout = new QVBoxLayout;
    infoLayout->setContentsMargins(0,0,0,0);
    infoLayout->addWidget(timeLabel);
    infoLayout->addWidget(infoLabel);
    infoWidget->setLayout(infoLayout);

    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSpacing(3);
    gridLayout->setRowStretch(0,1);
    gridLayout->addWidget(iconWidget,1,0,1,1);
    gridLayout->addWidget(middleWidget,1,1,1,1);
    gridLayout->addWidget(infoWidget,1,2,1,1);
    gridLayout->setRowStretch(3,1);

    topViewLabel = new QLabel(contentWidget);
    topViewLabel->setFixedSize(10,10);
    topViewLabel->move(0,0);

    contentWidget->setLayout(gridLayout);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    contentWidget->installEventFilter(q_ptr);
    topViewLabel->installEventFilter(q_ptr);
    descLabel->installEventFilter(q_ptr);
}

ToolItem::ToolItem(ToolPage *page, QWidget *parent) :
    QWidget(parent),
    d_ptr(new ToolItemPrivate(this))
{
    d_ptr->pagePtr = page;
    setFixedHeight(TOOL_ITEM_MAX_HEIGHT);
}

ToolItem::~ToolItem()
{

}

void ToolItem::setIcon(QString icon)
{
    MQ_D(ToolItem);
    d->iconLabel->setPixmap(icon);
}

void ToolItem::setName(QString name)
{
    MQ_D(ToolItem);
    d->nameLabel->setText(name);
}

QString ToolItem::getName() const
{
    MQ_D(ToolItem);
    return d->nameLabel->text();
}

void ToolItem::setNickName(QString name)
{
    MQ_D(ToolItem);
    d->nickLabel->setText(QString("(%1)").arg(name));
}

void ToolItem::setDescInfo(QString text)
{
    MQ_D(ToolItem);
    d->originDescInfo = text;
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
        if(event->type() == QEvent::Enter){
            if(!d->checked){
                setItemState(Mouse_Enter);
                return true;
            }
        }else if(event->type() == QEvent::Leave){
            if(!d->checked)
            {
                setItemState(Mouse_Leave);
                return true;
            }
        }else if(event->type() == QEvent::MouseButtonPress){
            emit itemButtonClick(QEvent::MouseButtonPress,this);
        }else if(event->type() == QEvent::MouseButtonRelease){
            QMouseEvent * e = dynamic_cast<QMouseEvent *>(event);
            if(e->button() == Qt::LeftButton || e->button() == Qt::RightButton)
            {
                setChecked(true);
                emit clearSelectionOthers(this);
                return true;
            }
        }else if(event->type() == QEvent::ContextMenu){
            if(d->contenxMenu)
            {
                emit updateGroupActions();
                emit itemButtonClick(QEvent::ContextMenu,this);
                d->contenxMenu->exec(QCursor::pos());
                return true;
            }
        }else if(event->type() == QEvent::MouseButtonDblClick){
            emit itemDoubleClick(this);
            return true;
        }
    }else if(watched == d->topViewLabel){
        if(event->type() == QEvent::Paint){
            if(d->topView){
                QPainter painter(d->topViewLabel);
                painter.setPen(Qt::NoPen);
                painter.setBrush(Qt::red);
                QPolygon polygon(3);
                polygon.setPoint(0,QPoint(0,0));
                polygon.setPoint(2,QPoint(d->topViewLabel->width(),0));
                polygon.setPoint(1,QPoint(0,d->topViewLabel->height()));
                painter.drawPolygon(polygon);
                return true;
            }
        }
    }else if(watched == d->descLabel){
        if(event->type() == QEvent::Paint){
            QPainter painter(d->descLabel);
            painter.drawText(d->descLabel->rect(),RUtil::replaceLongTextWidthElide(d->descLabel->font(),d->originDescInfo,d->descLabel->width()));
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
        default:
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

/*!
 * @brief item为历史记录时，设置infoLabel显示日期
 * @param[in] desc 待显示的日期内容
 */
void ToolItem::setDate(QString desc)
{
    MQ_D(ToolItem);
    d->timeLabel->setText(desc);
}

/*!
 * @brief 添加一条消息显示
 * @details 当ToolItem作为NotifyWindow中显示通知消息，infoLabel用于记录当前联系人消息的数量
 * @param[in] 无
 * @return 无
 */
void ToolItem::addNotifyInfo()
{
    MQ_D(ToolItem);
    if(d->notifyCount == 0)
    {
        QMetaEnum metaEnum = QMetaEnum::fromType<ItemInfoLabelType>();
        d->infoLabel->setProperty(metaEnum.name(),metaEnum.key(ItemSystemInfo));
        style()->unpolish(d->infoLabel);
        style()->polish(d->infoLabel);
    }
    d->updateNotifyInfoCount();
}

int ToolItem::getNotifyCount()
{
    MQ_D(ToolItem);
    return d->notifyCount;
}

bool ToolItem::isChecked()
{
    MQ_D(ToolItem);
    return d->checked;
}

/*!
 * @brief 设置item置顶显示
 * @param[in] flag 设置置顶状态
 */
void ToolItem::setTop(bool flag)
{
    MQ_D(ToolItem);
    d->topView = flag;
    update();
}

bool ToolItem::isTop()
{
    MQ_D(ToolItem);
    return d->topView;
}

void ToolItem::setChecked(bool flag)
{
    MQ_D(ToolItem);
    d->checked = flag;
    if(flag)
        setItemState(Mouse_Checked);
    else
        setItemState(Mouse_Leave);
}

/*!
 * @brief 根据在线状态设置状态显示
 * @param[in] status:OnlineStatus,联系人的在线状态枚举值
 * @return 无
 */
void ToolItem::setStatus(OnlineStatus status)
{
    MQ_D(ToolItem);
    d->onlineStatus = status;

    if(isOnline())
    {
        QString t_filePath = d->iconLabel->getPixmapFileInfo().absoluteFilePath();
        d->iconLabel->setPixmap(QPixmap(t_filePath));
        d->onLineStateLabel->setPixmap(QPixmap(OnLineState::getStatePixmap(status)).scaled(d->onLineStateLabel->width(),
                                                                                           d->onLineStateLabel->height()));
    }
    else
    {
        d->iconLabel->setGray(true);
        d->onLineStateLabel->setPixmap(QPixmap(""));
    }
}

bool ToolItem::isOnline() const
{
    MQ_D(ToolItem);
    return !(d->onlineStatus == STATUS_OFFLINE || d->onlineStatus == STATUS_HIDE);
}

ToolPage *ToolItem::toolpage()
{
    MQ_D(ToolItem);
    return d->pagePtr;
}

void ToolItem::setToolPage(ToolPage *page)
{
    MQ_D(ToolItem);
    if(page == NULL)
        return;
    d->pagePtr = page;
}

void ToolItem::cursorHoverIcon(bool flag)
{
    emit itemMouseHover(flag,this);
}

