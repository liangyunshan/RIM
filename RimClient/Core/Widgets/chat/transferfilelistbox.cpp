#include "transferfilelistbox.h"

#include <QList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

#include "head.h"
#include "transferfileitem.h"
#include "../../protocol/datastruct.h"

class TransferFileListBoxPrivate : public GlobalData<TransferFileListBox>
{
    Q_DECLARE_PUBLIC(TransferFileListBox)

protected:
    TransferFileListBoxPrivate(TransferFileListBox * q):q_ptr(q)
    {
        initWidget();
    }

    void initWidget();

    TransferFileListBox * q_ptr;
    QWidget * contentWidget;

    QList<TransferFileItem *> fileItems;
    QScrollArea * scrollArea;
};

void TransferFileListBoxPrivate::initWidget()
{
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    q_ptr->setLayout(mainLayout);

    scrollArea = new QScrollArea(q_ptr);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainLayout->addWidget(scrollArea);

    contentWidget = new QWidget(scrollArea);
    contentWidget->setObjectName("FileListBox_ContentWidget");

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addStretch(1);
    contentWidget->setLayout(contentLayout);

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);
}

TransferFileListBox::TransferFileListBox(QWidget *parent) :
    QWidget(parent),
    d_ptr(new TransferFileListBoxPrivate(this))
{

}

/*!
 * @brief 添加一个文件传输item
 * @param item 待添加的文件传输item
 */
void TransferFileListBox::addItem(TransferFileItem *item)
{
    MQ_D(TransferFileListBox);

    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());
    layout->insertWidget(d->contentWidget->children().size() - 1,item);
    d->fileItems.append(item);
}

/*!
 * @brief 获取当前列表中全部的文件传输item
 * @return 当前列表中全部的文件传输item集合
 */
QList<TransferFileItem *> TransferFileListBox::items() const
{
    MQ_D(TransferFileListBox);

    return d->fileItems;
}

/*!
 * @brief 清空当前列表中的全部文件传输item
 */
void TransferFileListBox::clear()
{
    MQ_D(TransferFileListBox);

    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());

    QLayoutItem * item = NULL;
    while((item = layout->takeAt(0))!= NULL)
    {
        if(item->widget())
        {
            delete item->widget();
        }
    }
    d->fileItems.clear();
    layout->addStretch(1);
}

/*!
 * @brief 移除目标文件传输item
 * @param item 目标文件传输item
 * @return
 */
bool TransferFileListBox::removeItem(TransferFileItem *item)
{
    MQ_D(TransferFileListBox);

    if(d->fileItems.size() > 0)
    {
        QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->contentWidget->layout());

        if(layout)
        {
            int index = -1;
            for(int i = 0; i < layout->count();i++)
            {
                if(layout->itemAt(i)->widget())
                {
                    TransferFileItem * tmpItem = dynamic_cast<TransferFileItem *>(layout->itemAt(i)->widget());
                    if(tmpItem == item)
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
                    bool removeResult = d->fileItems.removeOne(item);
                    if(removeResult)
                    {
                        delete layItem->widget();
                    }
                    return removeResult;
                }
            }
        }
    }
}

/*!
 * @brief 获取文件传输列表中文件传输item个数
 * @return 文件传输列表中文件传输item个数
 */
int TransferFileListBox::count() const
{
    MQ_D(TransferFileListBox);

    return d->fileItems.size();
}
