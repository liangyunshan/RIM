#include "toolpage.h"

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "toolbox.h"
#include "../actionmanager/actionmanager.h"
#include "Util/rutil.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QLabel>
#include <QStyle>
#include <QContextMenuEvent>
#include <QFontMetrics>
#include <QMenu>
#include <QPoint>
#include <QUuid>
#include <QDrag>
#include <QMimeData>

#define TOOL_SIMPLE_HEIGHT  26      //关闭状态下，标签默认高度

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

    QLabel * iconLabel;             /*!< 显示页面折叠的图标，三角图标 */
    QLabel * textLabel;             /*!< 显示分组名称 */
    QLabel * descLabel;             /*!< 显示分组的描述，若为联系人页面显示[在线/总人数],群页面显示[当前分组群数量] */

    QMenu * contextMenu;

    QList<ToolItem *> toolItems;

    bool expanded;
    PersonGroupInfo m_pageInfo;
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
    //解决在中文分组名称时，与descLabel间空格过大问题
    textLabel->setStyleSheet("background-color:rgba(0,0,0,0)");

    descLabel = new QLabel;
    descLabel->setObjectName("Tool_DescTextLabel");

    simpleLayout->addWidget(iconLabel);
    simpleLayout->addWidget(textLabel);
    simpleLayout->addSpacing(8);
    simpleLayout->addWidget(descLabel);
    simpleLayout->addStretch();
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
    d_ptr->m_pageInfo.uuid = RUtil::UUID();

}

ToolPage::~ToolPage()
{

}

/*!
 * @brief 设置工具页面内容
 * @param[in] text 待设置内容
 */
void ToolPage::setToolName(const QString &text)
{
    MQ_D(ToolPage);
    d->setToolName(text);
}

/*!
 * @brief 获取textLabel的显示内容
 * @return 指定页面textLabel内容
 */
QString ToolPage::toolName() const
{
    MQ_D(ToolPage);
    return d->textLabel->text();
}

/*!
 * @brief 设置page的id
 * @param [in]page的id
 */
void ToolPage::setID(const QString & id)
{
    MQ_D(ToolPage);
    d->m_pageInfo.uuid = id;
}

/*!
 * @brief 获取page的id
 * @return page的id
 */
QString ToolPage::getID() const
{
    MQ_D(ToolPage);
    return d->m_pageInfo.uuid;
}

/*!
 * @brief 设置page是否是默认分组
 * @param [in] isDefault:bool
 */
void ToolPage::setDefault(const bool isDefault)
{
    MQ_D(ToolPage);
    d->m_pageInfo.isDefault = isDefault;
}

/*!
 * @brief 获取page是否是默认分组
 * @return page是否是默认分组
 */
bool ToolPage::isDefault() const
{
    MQ_D(ToolPage);
    return d->m_pageInfo.isDefault;
}

void ToolPage::addItem(ToolItem * item)
{
    MQ_D(ToolPage);

    if(!d->detailWidget->layout()){
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

bool ToolPage::containsItem(std::function<bool(ToolItem *)> func)
{
    MQ_D(ToolPage);
    return  (std::find_if(d->toolItems.begin(),d->toolItems.end(),func) != d->toolItems.end());
}

/*!
 * @brief 删除目标item
 * @param[in] item:ToolItem *待删除的目标item
 * @return item是否从布局中删除成功
 * @details 将item从布局中删除、从toolItems列表中移除，但不销毁内存中的数据
 *          从布局中移除成功后发射itemRemoved(ToolItem*)信号，通知相应的
 *          ToolBox更新currentItem：ToolItem *
 */
bool ToolPage::removeItem(ToolItem *item)
{
    MQ_D(ToolPage);
    if(d->toolItems.size() > 0)
    {
        QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->detailWidget->layout());

        if(layout)
        {
            int index = -1;
            for(int i = 0; i < layout->count();i++){
                if(layout->itemAt(i)->widget()){
                    ToolItem * tmpItem = dynamic_cast<ToolItem *>(layout->itemAt(i)->widget());
                    if(tmpItem == item){
                        index = i;
                        break;
                    }
                }
            }

            if(index >= 0){
                QLayoutItem * layItem = layout->takeAt(index);
                if(layItem->widget()){
                    bool removeResult = d->toolItems.removeOne(item);
                    if(removeResult){
                        emit itemRemoved(item);
                    }
                    return removeResult;
                }
            }
        }
    }

    return false;
}

void ToolPage::setMenu(QMenu *menu)
{
    MQ_D(ToolPage);
    d->contextMenu = menu;
}

/*!
 * @brief 设置分组展开或闭合
 * @param[in] ifExpand:const bool
 */
void ToolPage::setExpand(bool ifExpand)
{
    MQ_D(ToolPage);
    d->setExpanded(ifExpand);
}

/*!
 * @brief 获取page是否是展开状态
 * @return page的展开状态
 */
bool ToolPage::isExpanded() const
{
    MQ_D(ToolPage);
    return d->expanded;
}

/*!
 * @brief 设置分组描述信息,用于显示联系人在线数量或群数量
 * @param[in] toolButton 待插入的工具按钮
 */
void ToolPage::setDescInfo(const QString &content)
{
    MQ_D(ToolPage);
    d->descLabel->setText(content);
}

/*!
 * @brief 获取textLabel的显示尺寸
 * @return
 */
QRect ToolPage::textRect() const
{
    MQ_D(ToolPage);
    return d->textLabel->geometry();
}

QRect ToolPage::titleRect() const
{
    MQ_D(ToolPage);
    return d->simpleTextWidget->geometry();
}

/*!
 * @brief 获取textLabel的默认固定高度
 * @return textLabel的默认固定高度
 */
int ToolPage::txtFixedHeight()
{
    return TOOL_SIMPLE_HEIGHT;
}

/*!
 * @brief 获取page的pageInfo(分组唯一标识、分组序号、分组名称)
 * @return const PersonGroupInfo &
 */
const PersonGroupInfo & ToolPage::pageInfo()
{
    MQ_D(ToolPage);
    return d->m_pageInfo;
}

/*!
 * @brief 高亮显示标题框
 */
void ToolPage::highlightShow()
{
    MQ_D(ToolPage);
    d->simpleTextWidget->setProperty("enter",true);
    style()->unpolish(d->simpleTextWidget);
    style()->polish(d->simpleTextWidget);
}

/*!
 * @brief 取消高亮显示标题框
 */
void ToolPage::unHighlightShow()
{
    MQ_D(ToolPage);
    d->simpleTextWidget->setProperty("enter",false);
    style()->unpolish(d->simpleTextWidget);
    style()->polish(d->simpleTextWidget);
}

/*!
 * @brief 判断Page中simpleTextWidget是否包含位置点
 * @param[in] point 判断包含位置点
 * @return 是否包含当前点
 */
bool ToolPage::containsInSimpleTextWidget(QPoint point)
{
    MQ_D(ToolPage);
    return d->simpleTextWidget->geometry().contains(point);
}

/*!
 * @brief 处理Item的SIGNAL：updateGroupActions()
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
                d->setExpanded(!d->expanded);
                emit selectedPage(this);
            }
            else if(e->button() == Qt::RightButton)
            {
                emit selectedPage(this);
            }

        }
        else if(event->type() == QEvent::ContextMenu)
        {
            if(ActionManager::instance()->action(Constant::ACTION_PANEL_PERSON_DELGROUP))
                ActionManager::instance()->action(Constant::ACTION_PANEL_PERSON_DELGROUP)->setEnabled(!(this->isDefault()));

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
