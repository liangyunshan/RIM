#include "toolbox.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMenu>
#include <QPointer>
#include <QContextMenuEvent>
#include <QAction>
#include <QCursor>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

#include "head.h"
#include "constants.h"
#include "datastruct.h"

using namespace GroupPerson;

class ToolBoxPrivate : public GlobalData<ToolBox>
{
    Q_DECLARE_PUBLIC(ToolBox)

private:
    ToolBoxPrivate(ToolBox * q):
        q_ptr(q)
    {
        initWidget();
        menu = NULL;
        currentPage = NULL;
        currentItem = NULL;
    }

    void initWidget();
    void addPage(ToolPage * page);

    ToolBox * q_ptr;
    QScrollArea * scrollArea;
    QWidget * contentWidget;

    QList<ToolPage *> pages;

    ToolPage * currentPage;
    ToolItem * currentItem;

    QMenu * menu;

    bool m_pressed;
};

void ToolBoxPrivate::initWidget()
{
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    q_ptr->setLayout(mainLayout);

    scrollArea = new QScrollArea(q_ptr);
    mainLayout->addWidget(scrollArea);

    contentWidget = new QWidget(scrollArea);
    contentWidget->setObjectName("ToolBox_ContentWidget");

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    m_pressed = false;//FIXME LYS
}

void ToolBoxPrivate::addPage(ToolPage *page)
{
    if(!contentWidget->layout())
    {
        QVBoxLayout * contentLayout = new QVBoxLayout;
        contentLayout->setContentsMargins(0,0,0,0);
        contentLayout->setSpacing(0);
        contentLayout->addStretch(1);
        contentWidget->setLayout(contentLayout);
    }

    QVBoxLayout * contentLayout = dynamic_cast<QVBoxLayout *>(contentWidget->layout());
    contentLayout->insertWidget(contentWidget->children().size() - 1,page);

    pages.append(page);
}

ToolBox::ToolBox(QWidget *parent) :
    d_ptr(new ToolBoxPrivate(this)),
    QWidget(parent)
{
    setAcceptDrops(true);
}

ToolPage * ToolBox::addPage(QString text)
{
    MQ_D(ToolBox);

    ToolPage * page = new ToolPage(this);
    connect(page,SIGNAL(clearItemSelection(ToolItem*)),this,SLOT(clearItemSelection(ToolItem*)));
    connect(page,SIGNAL(selectedPage(ToolPage*)),this,SLOT(setSlectedPage(ToolPage*)));
    connect(page,SIGNAL(updateGroupActions(ToolPage*)),this,SLOT(setGroupActions(ToolPage*)));
    connect(page,SIGNAL(itemRemoved(ToolItem*)),this,SLOT(itemRemoved(ToolItem*)));
    page->setToolName(text);

    d->addPage(page);
    setSlectedPage(page);

    return page;
}

/*!
     * @brief 移除targetPage
     * @details
     * @param[in] targetPage:ToolPage *
     * @return 移除targetPage结果
     */
bool ToolBox::removePage(ToolPage *targetPage)
{
    MQ_D(ToolBox);
    if(d->pages.size() > 0)
    {
        QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());

        if(layout)
        {
            int index = -1;
            for(int i = 0; i < layout->count();i++)
            {
                if(layout->itemAt(i)->widget())
                {
                    ToolPage * t_page = dynamic_cast<ToolPage *>(layout->itemAt(i)->widget());
                    if(t_page == targetPage)
                    {
                        index = i;
                        break;
                    }
                }
            }

            if(index >= 0)
            {
                QLayoutItem * layItem = layout->takeAt(index);
                if(layItem->widget())
                {
                    bool removeResult = d->pages.removeOne(targetPage);
                    return removeResult;
                }
            }
        }
    }

    return false;
}

ToolPage *ToolBox::selectedPage()
{
    MQ_D(ToolBox);
    return d->currentPage;
}

/*!
   @attention 在删除item后，需要及时将当前的item置为NULL
*/
ToolItem *ToolBox::selectedItem()
{
    MQ_D(ToolBox);
    return d->currentItem;
}

int ToolBox::pageCount()
{
    MQ_D(ToolBox);
    return d->pages.size();
}

void ToolBox::setContextMenu(QMenu *menu)
{
    MQ_D(ToolBox);
    d->menu = menu;
}

/*!
     * @brief 获取所有分组名称
     * @param[in] 无
     * @return 包含所有分组名称的QStringList
     *
     */

const QList<PersonGroupInfo> ToolBox::toolPagesinfos()
{
    MQ_D(ToolBox);
    QList <PersonGroupInfo> infoList;
    for(int index=0;index<pageCount();index++)
    {
        infoList.append(d->pages.at(index)->pageInfo());
    }
    return infoList;
}

/*!
     * @brief 倒数第二个page
     * @param[in] 无
     * @return 倒数第二个page
     *
     */
ToolPage *ToolBox::penultimatePage()
{
    MQ_D(ToolBox);
    //注：联系人分组列表中必须保证有一个默认分组
    return d->pages.at(d->pages.count()-2);
}

/*!
     * @brief 获取目标uuid的page
     * @param[in] 无
     * @return 匹配目标uuid的page
     *
     */
ToolPage *ToolBox::targetPage(QString &target)
{
    MQ_D(ToolBox);
    ToolPage * targetPage= NULL;
    for(int index=0;index<d->pages.count();index++)
    {
        QString uuid = d->pages.at(index)->pageInfo().uuid;
        if(target == uuid)
        {
            targetPage = d->pages.at(index);
        }
    }
    return targetPage;
}

void ToolBox::setSlectedPage(ToolPage *item)
{
    MQ_D(ToolBox);
    d->currentPage = item;
}

void ToolBox::clearItemSelection(ToolItem * item)
{
    MQ_D(ToolBox);

    d->currentItem = item;

    foreach(ToolPage * page,d->pages)
    {
        QList<ToolItem *>::iterator iter =  page->items().begin();
        while(iter != page->items().end())
        {
            if((*iter) != item && (*iter)->isChecked())
            {
                (*iter)->setChecked(false);
            }
            iter++;
        }
    }
}

/*!
     * @brief 处理page的SIGNAL：updateGroupActions(ToolPage *)
     * @param[in] page:ToolPage *,信源page
     * @return
     *
     */
void ToolBox::setGroupActions(ToolPage *page)
{
    emit updateGroupActions(page);
}

/*!
     * @brief 处理page的SIGNAL：itemRemoved(ToolItem*)
     * @param[in] removedItem:ToolItem *,被删除的item
     * @return
     *
     */
void ToolBox::itemRemoved(ToolItem * removedItem)
{
    MQ_D(ToolBox);
    if(removedItem == d->currentItem)
    {
        d->currentItem = NULL;
    }
}

/*!
     * @brief 根据给定鼠标拖动释放的位置判断page应该在布局中插入的位置
     *
     * @param[in] movedY:int,鼠标拖动释放的位置；
     *            sortedIndex：int&，保存page应该在布局中插入的位置
     *
     * @details 该算法还存在缺陷，待补充完整20180131
     *
     * @return 无
     *
     */
void ToolBox::indexInLayout(int movedY, int &sortedIndex)
{
    MQ_D(ToolBox);
    QList<int> pagesTopY;
    for(int t_index=0;t_index<d->pages.count();t_index++)
    {
        ToolPage * t_indexPage = d->pages.at(t_index);
        pagesTopY.append(t_indexPage->geometry().topLeft().y());
    }
    int t_count = pagesTopY.count();
    for(int t_index=t_count-1;t_index>0;t_index--)
    {
        int t_forward = pagesTopY.at(t_index);
        int t_prev = pagesTopY.at(t_index-1);
        if(t_forward>movedY && t_prev<movedY)
        {
            sortedIndex = t_index;
        }
    }
}

void ToolBox::contextMenuEvent(QContextMenuEvent *)
{
    MQ_D(ToolBox);
    if(d->menu)
    {
        d->menu->exec(QCursor::pos());
    }
}

void ToolBox::mousePressEvent(QMouseEvent *event)
{
    MQ_D(ToolBox);
    d->m_pressed = true;
    QWidget::mouseMoveEvent(event);
}

void ToolBox::mouseMoveEvent(QMouseEvent *event)
{
    MQ_D(ToolBox);
    if(d->m_pressed)
    {
        ToolPage * t_movedPage = selectedPage();    //FIXME LYS

        QPoint t_hotPot = event->pos() - t_movedPage->pos();

        QMimeData *t_MimData = new QMimeData;
        t_MimData->setText(t_movedPage->toolName());

        QPixmap t_pixMap(t_movedPage->titleRect().size());
        d->currentPage->render(&t_pixMap);

        QDrag *t_drag = new QDrag(this);
        t_drag->setMimeData(t_MimData);
        t_drag->setPixmap(t_pixMap);
        t_drag->setHotSpot(t_hotPot);

        Qt::DropAction dropAction = t_drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
        Q_UNUSED(dropAction);
    }
    QWidget::mouseMoveEvent(event);
}

void ToolBox::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
    {
        if (event->source() == this)
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }
        else
        {
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
}

void ToolBox::dropEvent(QDropEvent *event)
{
    MQ_D(ToolBox);
    int t_curY = event->pos().y();
    ToolPage * t_movedPage = d->currentPage;
    if(t_movedPage)
    {
        int t_movedIndex = -1;
        indexInLayout(t_curY,t_movedIndex);
        if(t_movedIndex != -1)
        {
            removePage(t_movedPage);
            QVBoxLayout * t_layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());
            if(t_layout)
            {
                t_layout->insertWidget(t_movedIndex,t_movedPage);
            }
        }
    }
    d->m_pressed = false;
}
