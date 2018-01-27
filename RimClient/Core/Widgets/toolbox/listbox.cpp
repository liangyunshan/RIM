#include "listbox.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QEvent>
#include <QMenu>
#include <QList>

#include "head.h"
#include "constants.h"
#include "datastruct.h"

class ListBoxPrivate : public GlobalData<ListBox>
{
    Q_DECLARE_PUBLIC(ListBox)

protected:
    ListBoxPrivate(ListBox * q):q_ptr(q)
    {
        initWidget();
        contextMenu = NULL;
        currentItem = NULL;
    }

    void initWidget();

    ListBox * q_ptr;
    QWidget * contentWidget;

    QList<ToolItem *> toolItems;
    QScrollArea * scrollArea;

    ToolItem * currentItem;

    QMenu * contextMenu;
};

void ListBoxPrivate::initWidget()
{
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    q_ptr->setLayout(mainLayout);

    scrollArea = new QScrollArea(q_ptr);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainLayout->addWidget(scrollArea);

    contentWidget = new QWidget(scrollArea);
    contentWidget->setObjectName("ToolBox_ContentWidget");

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addStretch(1);
    contentWidget->setLayout(contentLayout);

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
}

ListBox::ListBox(QWidget *parent) : QWidget(parent),
    d_ptr(new ListBoxPrivate(this))
{
    d_ptr->contentWidget->installEventFilter(this);
}

void ListBox::addItem(ToolItem * item)
{
    MQ_D(ListBox);

    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());
    connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),this,SLOT(clearItemSelection(ToolItem *)));

    layout->insertWidget(d->contentWidget->children().size() - 1,item);
    d->toolItems.append(item);
}

QList<ToolItem *> ListBox::items() const
{
    MQ_D(ListBox);

    return d->toolItems;
}

bool ListBox::removeItem(ToolItem *item)
{
    MQ_D(ListBox);

    if(d->toolItems.size() > 0)
    {
        QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());

        if(layout)
        {
            int index = -1;
            for(int i = 0; i < layout->count();i++)
            {
                if(layout->itemAt(i)->widget() && layout->itemAt(i)->widget() == item)
                {
                    index = i;
                    break;
                }
            }
            if(index >= 0)
            {
                QLayoutItem * layItem = layout->takeAt(index);
                if(layItem->widget())
                {
                    d->toolItems.removeOne(item);
                    delete layItem->widget();
                }
            }
        }
    }

    return false;
}

ToolItem *ListBox::selectedItem()
{
    MQ_D(ListBox);
    return d->currentItem;
}

void ListBox::setContextMenu(QMenu *menu)
{
    MQ_D(ListBox);
    d->contextMenu = menu;
}

void ListBox::clear()
{
    MQ_D(ListBox);
    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());

    QLayoutItem * item = NULL;
    while((item = layout->takeAt(0))!= NULL)
    {
        if(item->widget())
        {
            delete item->widget();
        }
    }

    d->toolItems.clear();
    layout->addStretch(1);
}

void ListBox::clearItemSelection(ToolItem *item)
{
    MQ_D(ListBox);

    d->currentItem = item;

    QList<ToolItem *>::iterator iter =  d->toolItems.begin();
    while(iter != d->toolItems.end())
    {
        if((*iter) != item && (*iter)->isChecked())
        {
            (*iter)->setChecked(false);
        }
        iter++;
    }

    emit currentItemChanged(item);
}

bool ListBox::eventFilter(QObject *watched, QEvent *event)
{
    return QWidget::eventFilter(watched,event);
}

void ListBox::contextMenuEvent(QContextMenuEvent *)
{
    MQ_D(ListBox);
    if(d->contextMenu)
    {
        d->contextMenu->exec(QCursor::pos());
    }
}
