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
#include <QApplication>
#include <QPainter>

#include <QTIme>

#include "head.h"
#include "constants.h"
#include "datastruct.h"

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

    QList<ToolPage *> pages;                        /*!< 装载多个信息页面 */

    ToolPage * currentPage;
    ToolItem * currentItem;

    QMenu * menu;

    bool m_leftPressed;
    QPoint m_startPos;
    QList<int> m_laterExpandPages;
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

    m_leftPressed = false;//FIXME LYS
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
    connect(this,SIGNAL(noticePageRemoved(ToolPage *)),this,SLOT(pageRemoved(ToolPage *)));
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
     * @details 仅将目标page从布局中移除但仍保留在pages列表中
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
//                    bool removeResult = d->pages.removeOne(targetPage);
//                    return removeResult;
                    emit noticePageRemoved(targetPage);
                    return true;
                }
            }
        }
    }

    return false;
}

bool ToolBox::removeFromList(ToolPage *targetPage)
{
    MQ_D(ToolBox);
    bool t_result = d->pages.removeOne(targetPage);
    if(t_result)
    {
        delete targetPage;
    }
    return t_result;
}

/*!
 * @brief 重新排列d->pages的顺序
 * @attention 此方法需要在移动分组后，接收服务器响应后更新本地的page中的分组。
 * @param[in] groupId 待排序的分组id
 * @param[in] newPageIndex 已排的序号
 */
void ToolBox::sortPage(const QString &groupId, int newPageIndex)
{
    MQ_D(ToolBox);
    int index = -1;
    QList<ToolPage *>::iterator iter = std::find_if(d->pages.begin(),d->pages.end(),[&](ToolPage * page){
        index++;
        return page->pageInfo().uuid == groupId;
    });

    if(iter != d->pages.end() && index >=0 && index < d->pages.size() && newPageIndex < d->pages.size()){
        ToolPage * tmpPage = d->pages.takeAt(index);
        d->pages.insert(newPageIndex,tmpPage);
    }
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

QList<ToolPage *>& ToolBox::allPages() const
{
    MQ_D(ToolBox);
    return d->pages;
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
 * @brief 倒数第二个page,插入新page前最后一个page的索引
 * @warning 联系人分组列表中必须保证有一个默认分组
 * @return 倒数第二个page
 */
ToolPage *ToolBox::penultimatePage()
{
    MQ_D(ToolBox);
    return d->pages.at(d->pages.count() - 2);
}

/*!
 * @brief 获取默认分组page
 * @param 无
 * @return 默认分组page
 */
ToolPage *ToolBox::defaultPage()
{
    MQ_D(ToolBox);
    ToolPage * t_defult = NULL;
    for(int t_index=0;t_index<pageCount();t_index++)
    {
        if(d->pages.at(t_index)->isDefault())
        {
            t_defult = d->pages.at(t_index);
            break;
        }
    }
    return t_defult;
}

/*!
 * @brief 获取目标uuid的page
 * @param[in] 无
 * @return 匹配目标uuid的page
 */
ToolPage *ToolBox::targetPage(QString &target)
{
    MQ_D(ToolBox);
    ToolPage * targetPage= NULL;

    QList<ToolPage *>::iterator iter = std::find_if(d->pages.begin(),d->pages.end(),[&target](ToolPage * page){
        return page->pageInfo().uuid == target;
    });

    if(iter != d->pages.end())
        targetPage = (*iter);

    return targetPage;
}

/*!
 * @brief 获取包含item的page
 * @param[in] 无
 * @return 包含目标item的page
 *
 */
ToolPage *ToolBox::targetPage(ToolItem * item)
{
    MQ_D(ToolBox);
    ToolPage * targetPage= NULL;
    for(int index=0;index<d->pages.count();index++)
    {
        if(d->pages.at(index)->items().contains(item))
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
 */
void ToolBox::setGroupActions(ToolPage *page)
{
    emit updateGroupActions(page);
}

/*!
 * @brief 处理page的SIGNAL：itemRemoved(ToolItem*)
 * @param[in] removedItem:ToolItem *,被删除的item
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
 * @brief 处理page的SIGNAL：pageRemoved(ToolPage*)
 * @param[in] removedPage:ToolPage *,被删除的page
 */
void ToolBox::pageRemoved(ToolPage * removedPage)
{
    MQ_D(ToolBox);
    if(removedPage == d->currentPage)
    {
        d->currentPage = NULL;
    }
}

/*!
 * @brief 根据给定鼠标拖动释放的位置判断page应该在布局中插入的位置
 * @param[in] pos:QPoint,鼠标拖动释放的位置；
 *            sortedIndex：int&，保存page应该在布局中插入的位置
 * @return 无
 */
void ToolBox::indexInLayout(QPoint pos, int &sortedIndex)
{
    MQ_D(ToolBox);
    if(pageInPos(pos))
    {
       int t_pageIndex = d->pages.indexOf(pageInPos(pos));
       int t_rowHeight = pageInPos(pos)->titleRect().height();
       if(pos.y()-t_pageIndex*t_rowHeight >= t_rowHeight/2){
           sortedIndex = t_pageIndex+1;
       }else{
           sortedIndex = t_pageIndex;
       }
    }else{
        sortedIndex = d->pages.count()-1;
    }

}

/*!
 * @brief 判断鼠标拖拽时是否是page,是则返回page，否则返回NULL
 * @param[in] pressedPos:const QPoint &(鼠标拖拽时点击的位置)
 * @details 将展开的page暂时闭合，结束取值后再展开
 * @return 鼠标拖拽的是page则返回page，否则返回NULL
 */
ToolPage *ToolBox::pageInPos(const QPoint & pressedPos)
{
    MQ_D(ToolBox);
    ToolPage * t_returnPage = NULL;
    QList <int> t_expandedPages;
    for(int t_index=0;t_index<pageCount();t_index++)
    {
        if(d->pages.at(t_index)->isExpanded()){
            t_expandedPages.append(t_index);
            d->pages.at(t_index)->setExpand(false);
        }

        if(d->pages.at(t_index)->geometry().contains(pressedPos)){
            t_returnPage = d->pages.at(t_index);
            break;
        }
    }
    for(int t_index=0;t_index<t_expandedPages.count();t_index++)
    {
        d->pages.at(t_expandedPages.at(t_index))->setExpand(true);
    }

    return t_returnPage;
}

/*!
 * @brief 仅高亮显示curpos表示的当前位置的page
 * @param[in] curpos:QPoint &,当前目标点
 * @return 无
 */
void ToolBox::highLightTarget(const QPoint & curpos)
{
    MQ_D(ToolBox);
    ToolPage * t_decoratePage = pageInPos(curpos);

//    auto highLight = [&](ToolPage * page){page->unHighlightShow();};
//    std::for_each(d->pages.begin(),d->pages.end(),highLight);
    std::for_each(d->pages.begin(),d->pages.end(),[&](ToolPage * page){page->unHighlightShow();});

    if(t_decoratePage){
        t_decoratePage->highlightShow();
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
    if(event->button()&Qt::LeftButton)
    {
        d->m_leftPressed = true;
        d->m_startPos = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void ToolBox::mouseReleaseEvent(QMouseEvent *event)
{
    MQ_D(ToolBox);
    if(event->button()&Qt::LeftButton)
    {
        d->m_leftPressed = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void ToolBox::mouseMoveEvent(QMouseEvent *event)
{
    MQ_D(ToolBox);
    if(d->m_leftPressed)
    {
        ToolPage * t_movedPage = pageInPos(d->m_startPos);    //FIXME LYS
        if(!t_movedPage)
        {
            return;
        }

        for(int t_index=0;t_index<pageCount();t_index++)
        {
            if(d->pages.at(t_index)->isExpanded())
            {
                d->m_laterExpandPages.append(t_index);
                d->pages.at(t_index)->setExpand(false);
            }
        }

        QPoint t_hotPot = d->m_startPos - t_movedPage->pos();

        QMimeData *t_MimData = new QMimeData;
        t_MimData->setText(t_movedPage->getID());

        QPixmap t_pixMap(t_movedPage->titleRect().size());
        t_movedPage->render(&t_pixMap);

        if((event->pos()-d->m_startPos).manhattanLength()<QApplication::startDragDistance())
        {
            QDrag *t_drag = new QDrag(this);
            t_drag->setMimeData(t_MimData);
            t_drag->setPixmap(t_pixMap);
            t_drag->setHotSpot(t_hotPot);

            Qt::DropAction dropAction = t_drag->exec(Qt::MoveAction);
            Q_UNUSED(dropAction);
        }
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void ToolBox::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasText()){
        if(event->source() == this){
            event->setDropAction(Qt::MoveAction);
            event->accept();
        }else{
            event->acceptProposedAction();
        }
    }else{
        QWidget::dragEnterEvent(event);
    }
}

void ToolBox::dragMoveEvent(QDragMoveEvent *event)
{
    MQ_D(ToolBox);
    if(d->m_leftPressed){
        highLightTarget(event->pos());
        event->acceptProposedAction();
    }else{
        QWidget::dragMoveEvent(event);
    }
}

/*!
 * @brief 响应分组放下操作
 * @note 20180419:【现象】修复快速拖动分组时分组显示在页面底部
 *                【原因】未判断事件源(t_movedPage)和目标事件(pageInPos())是否不一致，造成事件源自身捕获了事件
 */
void ToolBox::dropEvent(QDropEvent *event)
{
    MQ_D(ToolBox);
    ToolPage * t_movedPage = targetPage(event->mimeData()->text());
    if(t_movedPage)
    {
        int t_movedIndex = -1;

        if(pageInPos(event->pos()) == t_movedPage)
        {
            return;
        }

        indexInLayout(event->pos(),t_movedIndex);
        if(t_movedIndex != -1)
        {
            removePage(t_movedPage);
            QVBoxLayout * t_layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());
            if(t_layout)
            {
                if(t_movedIndex >= t_layout->count())
                    t_movedIndex -= 1;

                t_layout->insertWidget(t_movedIndex,t_movedPage);
                emit pageIndexMoved(t_movedIndex,t_movedPage->getID());
            }
        }
    }

    d->m_leftPressed = false;

    for(int t_index=0;t_index<d->m_laterExpandPages.count();t_index++){
        d->pages.at(d->m_laterExpandPages.at(t_index))->setExpand(true);
    }
    d->m_laterExpandPages.clear();

    QWidget::dropEvent(event);
}
