#include "toolbox.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMenu>
#include <QPointer>
#include <QContextMenuEvent>
#include <QAction>
#include <QCursor>

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

}

ToolPage * ToolBox::addPage(QString text)
{
    MQ_D(ToolBox);

    ToolPage * page = new ToolPage(this);
    connect(page,SIGNAL(clearItemSelection(ToolItem*)),this,SLOT(clearItemSelection(ToolItem*)));
    connect(page,SIGNAL(selectedPage(ToolPage*)),this,SLOT(setSlectedPage(ToolPage*)));
    connect(page,SIGNAL(currentPosChanged()),this,SLOT(updateLayout()));
    connect(page,SIGNAL(updateGroupActions(ToolPage*)),this,SLOT(setGroupActions(ToolPage*)));
    page->setToolName(text);

    d->addPage(page);
    setSlectedPage(page);

    return page;
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

void ToolBox::testMe()
{
    for(int i = 0; i < pageCount(); i++)
    {

    }
}

void ToolBox::setContextMenu(QMenu *menu)
{
    MQ_D(ToolBox);
    d->menu = menu;
}

/*!
     * @brief 获取所有分组名称
     *
     * @param[in] 无
     *
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
     *
     * @param[in] 无
     *
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
     *
     * @param[in] 无
     *
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
     * @brief 处理page移动后的布局
     *
     * @param[in] 无
     *
     * @return
     *
     */
//TODO LYS-20180122 在布局中移动分组进行排序
void ToolBox::updateLayout()
{

}

/*!
     * @brief 处理page的SIGNAL：updateGroupActions(ToolPage *)
     *
     * @param[in] page:ToolPage *,信源page
     *
     * @return
     *
     */
void ToolBox::setGroupActions(ToolPage *page)
{
    emit updateGroupActions(page);
}

void ToolBox::contextMenuEvent(QContextMenuEvent *)
{
    MQ_D(ToolBox);
    if(d->menu)
    {
        d->menu->exec(QCursor::pos());
    }
}

