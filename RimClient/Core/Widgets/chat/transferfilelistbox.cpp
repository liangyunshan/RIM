#include "transferfilelistbox.h"

#include <QList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

#include "head.h"
#include "transferfileitem.h"
#include "../../protocol/datastruct.h"
#include "../../messdiapatch.h"

class TransferFileListBoxPrivate : public GlobalData<TransferFileListBox>
{
    Q_DECLARE_PUBLIC(TransferFileListBox)

protected:
    explicit TransferFileListBoxPrivate(TransferFileListBox * q):q_ptr(q)
    {
        initWidget();
    }

    void initWidget();

    TransferFileListBox * q_ptr;
    QWidget * contentWidget;

    QList<TransferFileItem *> fileItems;
    QScrollArea * scrollArea;

    int m_transingCount = 0;    //正在传送的文件个数
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
    connect(item,SIGNAL(saveAsFile(TransferFileItem *)),this,SLOT(saveAsFile(TransferFileItem *)));
    connect(item,SIGNAL(toOffLineSend(TransferFileItem *)),this,SLOT(toOffLineSend(TransferFileItem *)));
    connect(item,SIGNAL(startRecvFile(TransferFileItem *)),this,SLOT(startRecvFile(TransferFileItem *)));
    connect(item,SIGNAL(cancelTransfer(TransferFileItem *)),this,SLOT(cancelTransfer(TransferFileItem *)));
    emit itemAdded();
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
    foreach (TransferFileItem *item, d->fileItems)
    {
        disconnect(item,SIGNAL(saveAsFile(TransferFileItem *)),this,SLOT(saveAsFile(TransferFileItem *)));
        disconnect(item,SIGNAL(toOffLineSend(TransferFileItem *)),this,SLOT(toOffLineSend(TransferFileItem *)));
        disconnect(item,SIGNAL(startRecvFile(TransferFileItem *)),this,SLOT(startRecvFile(TransferFileItem *)));
        disconnect(item,SIGNAL(cancelTransfer(TransferFileItem *)),this,SLOT(cancelTransfer(TransferFileItem *)));
    }
    d->fileItems.clear();
    layout->addStretch(1);
}

/*!
 * @brief 移除目标文件传输item
 * @param item 目标文件传输item
 * @return 移除结果
 */
bool TransferFileListBox::removeItem(TransferFileItem *item)
{
    MQ_D(TransferFileListBox);

    bool removeResult = false;
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
                    removeResult = d->fileItems.removeOne(item);
                    if(removeResult)
                    {
                        disconnect(item,SIGNAL(saveAsFile(TransferFileItem *)),this,SLOT(saveAsFile(TransferFileItem *)));
                        disconnect(item,SIGNAL(toOffLineSend(TransferFileItem *)),this,SLOT(toOffLineSend(TransferFileItem *)));
                        disconnect(item,SIGNAL(startRecvFile(TransferFileItem *)),this,SLOT(startRecvFile(TransferFileItem *)));
                        disconnect(item,SIGNAL(cancelTransfer(TransferFileItem *)),this,SLOT(cancelTransfer(TransferFileItem *)));
                        delete layItem->widget();
                        emit itemRemoved();
                    }
                }
            }
        }
    }
    return removeResult;
}

/*!
 * @brief 移除目标索引值的文件传输item
 * @param index 目标索引值
 * @return 移除结果
 */
bool TransferFileListBox::removeItem(int index)
{
    MQ_D(TransferFileListBox);

    bool removeResult = false;
    if(count()>index)
    {
        TransferFileItem *t_item = d->fileItems.at(index);
        removeResult = removeItem(t_item);
    }
    return removeResult;
}

/*!
 * @brief 获取目标索引值的item
 * @param index 目标索引值
 * @return 目标索引值的item
 */
TransferFileItem *TransferFileListBox::itemAt(int index) const
{
    MQ_D(TransferFileListBox);

    return d->fileItems.at(index);
}

/*!
 * @brief 获取目标item的索引值
 * @param item 目标item
 * @return 目标item的索引值
 */
int TransferFileListBox::indexOf(TransferFileItem *item) const
{
    MQ_D(TransferFileListBox);

    int index = 0;
    TransferFileItem *temp = d->fileItems.at(index);
    while (temp) {
        if (temp == item)
            return index;
        ++index;
        temp = d->fileItems.at(index);
    }
    return -1;
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

/*!
 * @brief 获取当前整个文件传输列表传送进度
 * @return 当前整个文件传输列表传送进度
 */
QString TransferFileListBox::taskListProgress()
{
    MQ_D(TransferFileListBox);

    QString t_transing = QString::number(d->m_transingCount);
    QString t_transCount = QString::number(count());
    return  QString(t_transing+"/"+t_transCount);
}

/*!
 * @brief 执行文件另存为操作
 * @param item 执行文件另存为操作的目标item
 */
void TransferFileListBox::saveAsFile(TransferFileItem *item)
{
    emit saveAsFile(item->taskSerialNo());
}

/*!
 * @brief 执行文件转为离线发送操作
 * @param item 执行文件转为离线发送操作的目标item
 */
void TransferFileListBox::toOffLineSend(TransferFileItem *item)
{
    emit toOffLineSend(item->taskSerialNo());
}

/*!
 * @brief 执行文件开始接收操作
 * @param item 执行文件开始接收操作的目标item
 */
void TransferFileListBox::startRecvFile(TransferFileItem *item)
{
    emit startRecvFile(item->taskSerialNo());
}

/*!
 * @brief 执行文件取消传送操作
 * @param item 执行文件取消传送操作的目标item
 */
void TransferFileListBox::cancelTransfer(TransferFileItem *item)
{
    emit cancelTransfer(item->taskSerialNo());
}

/*!
 * @brief 设置文件传输状态
 * @param progress 文件传输状态
 */
void TransferFileListBox::SetTransStatus(const FileTransProgress &progress)
{
    MQ_D(TransferFileListBox);

    TransferFileItem *t_item = NULL;
    foreach (t_item, d->fileItems)
    {
        if(t_item->taskSerialNo() == progress.serialNo)
        {
            break;
        }
    }
    if(t_item)
    {
        t_item->setFinishedSize(progress.readySendBytes);
        t_item->setFileSize(progress.totleBytes);
        if(progress.transStatus == FileTransStatus::TransStart)
        {
            t_item->setTaskIfStarted(true);
            d->m_transingCount ++;
            emit transferStatusChanged();
        }
        else if(progress.transStatus == FileTransStatus::TransCancel)
        {
            if(t_item->taskIfStarted() == true)
            {
                d->m_transingCount --;
                emit transferStatusChanged();
            }
        }
        else if(progress.transStatus == FileTransStatus::TransSuccess)
        {
            d->m_transingCount --;
            removeItem(t_item);
            emit transferStatusChanged();
        }
    }
}
