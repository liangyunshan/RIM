#include "toolpage.h"

#include "head.h"
#include "datastruct.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QLabel>
#include <QStyle>
#include <QContextMenuEvent>
#include <QFontMetrics>
#include <QMenu>
#include <QPoint>
#include <QUuid>

#include "toolbox.h"

#define TOOL_SIMPLE_HEIGHT  24

class ToolPagePrivate : public GlobalData<ToolPage>
{
    Q_DECLARE_PUBLIC(ToolPage)
private:
    ToolPagePrivate(ToolPage * q):q_ptr(q)
    {
        initWidget();
        contextMenu = NULL;
    }

    void initWidget();

    void setToolName(QString text);

    void setExpanded(bool flag);

    ToolPage * q_ptr;
    ToolBox * toolBox;

    QWidget * contentWidget;
    QWidget * simpleTextWidget;
    QWidget * detailWidget;

    QLabel * iconLabel;
    QLabel * textLabel;
    QLabel * descLabel;

    QMenu * contextMenu;

    QList<ToolItem *> toolItems;

    bool expanded;
    QPoint m_oldPoint;
    bool m_bButtonPressed;
    bool m_dragPage;
   GroupPerson::PersonGroupInfo m_pageInfo;
};

void ToolPagePrivate::initWidget()
{
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    contentWidget = new QWidget;

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    simpleTextWidget = new QWidget(contentWidget);
    simpleTextWidget->setObjectName("SimpleTextWidget");
    simpleTextWidget->setProperty("selected",false);
    simpleTextWidget->setFixedHeight(TOOL_SIMPLE_HEIGHT);

    detailWidget = new QWidget(contentWidget);

    contentLayout->addWidget(simpleTextWidget);
    contentLayout->addWidget(detailWidget);
    contentWidget->setLayout(contentLayout);

    QHBoxLayout * simpleLayout = new QHBoxLayout;
    simpleLayout->setContentsMargins(0,0,0,0);
    simpleLayout->setSpacing(0);

    iconLabel = new QLabel;
    iconLabel->setFixedSize(QSize(TOOL_SIMPLE_HEIGHT,TOOL_SIMPLE_HEIGHT));

    textLabel = new QLabel;
    textLabel->setFont(QFont("微软雅黑",13));
    textLabel->setObjectName("Tool_SimpleTextLabel");

    descLabel = new QLabel;
    textLabel->setObjectName("Tool_DescTextLabel");

    simpleLayout->addWidget(iconLabel);
    simpleLayout->addWidget(textLabel);
    simpleLayout->addWidget(descLabel);
    simpleTextWidget->setLayout(simpleLayout);

    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    setExpanded(false);
}

void ToolPagePrivate::setToolName(QString text)
{
    textLabel->setText(text);
    m_pageInfo.name = text;
    QFontMetrics fm(textLabel->font());
    textLabel->setFixedWidth(fm.width(textLabel->text()));
}

void ToolPagePrivate::setExpanded(bool flag)
{
    expanded = flag;
    detailWidget->setVisible(expanded);
    iconLabel->setStyleSheet("border-image:url("+
                             (expanded?QString(":/icon/resource/icon/Group_Expand_Tri.png")
                                     :QString(":/icon/resource/icon/Group_Close_Tri.png"))+")");

    if(!flag)
    {
        QList<ToolItem*>::iterator iter = toolItems.begin();
        while(iter != toolItems.end())
        {
            if((*iter)->isChecked())
            {
                (*iter)->setChecked(false);
            }

            iter++;
        }
    }
}


ToolPage::ToolPage(ToolBox *parent):
    d_ptr(new ToolPagePrivate(this)),
    QWidget(parent)
{
    d_ptr->toolBox = parent;
    d_ptr->detailWidget->installEventFilter(this);
    d_ptr->simpleTextWidget->installEventFilter(this);
    d_ptr->m_pageInfo.uuid = QUuid::createUuid().toString();
}

ToolPage::~ToolPage()
{

}

/*!
     * @brief 设置工具页面内容
     *
     * @param[in] text 待设置内容
     *
     * @return 无
     *
     */
void ToolPage::setToolName(QString text)
{
    MQ_D(ToolPage);
    d->setToolName(text);
}

void ToolPage::addItem(ToolItem * item)
{
    MQ_D(ToolPage);

    if(!d->detailWidget->layout())
    {
        QVBoxLayout * layout = new QVBoxLayout;
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        d->detailWidget->setLayout(layout);
    }

    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->detailWidget->layout());

    d->toolItems.append(item);

    layout->addWidget(item);
}

QList<ToolItem *>& ToolPage::items()
{
    MQ_D(ToolPage);
    return d->toolItems;
}

void ToolPage::setMenu(QMenu *menu)
{
    MQ_D(ToolPage);
    d->contextMenu = menu;
}

/*!
     * @brief 获取textLabel的显示尺寸
     *
     * @param 无
     *
     * @return 无
     *
     */
QRect ToolPage::textRect() const
{
    MQ_D(ToolPage);
    return d->textLabel->geometry();
}

/*!
     * @brief 获取textLabel的显示内容
     *
     * @param 无
     *
     * @return 无
     *
     */
QString ToolPage::toolName() const
{
    MQ_D(ToolPage);
    return d->textLabel->text();
}

/*!
     * @brief 获取textLabel的默认固定高度
     *
     * @param 无
     *
     * @return textLabel的默认固定高度
     *
     */
int ToolPage::txtFixedHeight()
{
    int fixedHeight = TOOL_SIMPLE_HEIGHT;
    return fixedHeight;
}

/*!
     * @brief 获取page的pageInfo(分组唯一标识、分组序号、分组名称)
     *
     * @param 无
     *
     * @return const PersonGroupInfo &
     *
     */
const PersonGroupInfo & ToolPage::pageInfo()
{
    MQ_D(ToolPage);
    return d->m_pageInfo;
}

/*!
     * @brief 设置page的sortNum(分组排序序号)
     *
     * @param [in]page的sortNum(分组排序序号)
     *
     * @return 无
     *
     */
void ToolPage::setSortNum(const int num)
{
    MQ_D(ToolPage);
    d->m_pageInfo.sortNum = num;
}

/*!
     * @brief 处理Item的SIGNAL：updateGroupActions()
     *
     * @param 无
     *
     * @return 无
     *
     */
void ToolPage::updateGroupActions()
{
    emit updateGroupActions(this);
}

bool ToolPage::eventFilter(QObject *watched, QEvent *event)
{
    MQ_D(ToolPage);

    if(watched == d->simpleTextWidget)
    {
        if(event->type() == QEvent::Enter)
        {
            d->simpleTextWidget->setProperty("enter",true);
            style()->unpolish(d->simpleTextWidget);
            style()->polish(d->simpleTextWidget);
        }
        else if(event->type() == QEvent::Leave)
        {
            d->simpleTextWidget->setProperty("enter",false);
            style()->unpolish(d->simpleTextWidget);
            style()->polish(d->simpleTextWidget);
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent * e = dynamic_cast<QMouseEvent *>(event);
            if(e->button() == Qt::LeftButton)
            {
                if(!d->m_dragPage)
                {
                    d->setExpanded(!d->expanded);
                }

            }
            else if(e->button() == Qt::RightButton)
            {
                emit selectedPage(this);
            }
        }
        else if(event->type() == QEvent::ContextMenu)
        {
            if(d->contextMenu)
            {
                d->contextMenu->exec(QCursor::pos());
            }

            return true;
        }
    }
    else if(watched == d->detailWidget)
    {
        if(event->type() == QEvent::ContextMenu)
        {
            return true;
        }
    }

    return QWidget::eventFilter(watched,event);
}

void ToolPage::mousePressEvent(QMouseEvent *e)
{
    MQ_D(ToolPage);
    if(e->button() == Qt::LeftButton)
    {
        d->m_oldPoint = e->pos();
        d->m_bButtonPressed = true;
        d->m_dragPage = false;
    }
}

void ToolPage::mouseMoveEvent(QMouseEvent *e)
{
    MQ_D(ToolPage);
    if(d->m_bButtonPressed)
    {
        if(d->expanded)
        {
            d->setExpanded(!d->expanded);
        }
        d->m_dragPage = true;
        QPoint oldPoint = d->m_oldPoint;
        QPoint moveToPoint = e->pos()-oldPoint+geometry().topLeft();
        QRect parentRect = d->toolBox->geometry();

        int moveTLX = moveToPoint.x();
        int moveTLY = moveToPoint.y();
        int moveBRX = moveToPoint.x()+this->width();
        int moveBRY = moveToPoint.y()+this->height();
        if(moveTLX<parentRect.topLeft().x())
        {
            moveToPoint.setX(parentRect.topLeft().x());
        }
        if(moveTLY<parentRect.topLeft().y())
        {
            moveToPoint.setY(parentRect.topLeft().y());
        }
        if(moveBRX>parentRect.bottomRight().x())
        {
            moveToPoint.setX(parentRect.topLeft().x());
        }
        if(moveBRY>parentRect.bottomRight().y())
        {
            moveToPoint.setY(parentRect.bottomRight().y()-this->height());
        }
        move(moveToPoint);
        emit currentPosChanged();
        oldPoint = moveToPoint;
    }
    QWidget::mouseMoveEvent(e);
}

void ToolPage::mouseReleaseEvent(QMouseEvent *e)
{
    MQ_D(ToolPage);
    if(e->button() == Qt::LeftButton)
    {
        d->m_bButtonPressed = false;
        d->m_dragPage = false;
    }
    QWidget::mouseReleaseEvent(e);
}
