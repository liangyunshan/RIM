#include "transferfileitem.h"

#include <QLabel>
#include <QFileInfo>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QDebug>

#include "util/rsingleton.h"

#include "../../thread/file716sendtask.h"
#include "../../head.h"

#define ITEM_MAX_HEIGHT 56

class TransferFileItemPrivate : GlobalData<TransferFileItem>
{
    Q_DECLARE_PUBLIC(TransferFileItem)

private:
    explicit TransferFileItemPrivate(TransferFileItem *q) :
        q_ptr(q)
    {
        m_fileType = 0;
        m_transferType = 0;
        m_finishedSize = 0;
        m_totalSize = 0;
        initWidget();
    }

    TransferFileItem * q_ptr;

    void initWidget();

    QWidget *contentWidget;
    QLabel * fileTypeLabel;             //文件类型图标
    QLabel * transferTypeLabel;         //文件传输类型（发送/接收）
    QLabel * fileNameLabel;             //文件名
    QLabel * fileSizeLabel;             //文件大小
    QProgressBar * transferProgress;    //文件传输进度
    QLabel * curTimeLabel;              //当前时间
    QPushButton * toOffLineBtn;         //转离线发送
    QPushButton * receiveBtn;           //接收
    QPushButton * saveAsBtn;            //另存为
    QPushButton * cancelBtn;            //取消

    int m_fileType,m_transferType;      //传输的文件类型、文件传输类型
    int m_finishedSize,m_totalSize;     //已完成传输大小、传输文件总大小
    SenderFileDesc m_fileDesc;
};

void TransferFileItemPrivate::initWidget()
{
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    contentWidget = new QWidget(q_ptr);
    contentWidget->setStyleSheet("QProgressBar {"
                                    "border: 1px solid #05B8CC;"
                                    "border-radius: 3px;"
                                    "text-align: center;"
                                    "height:3px;}"
                                 "QProgressBar::chunk {"
                                    "background-color: #05B8CC;}"
                                 "QPushButton {"
                                    "border: 0px;"
                                    "color:blue;"
                                    "background-color:transparent;}"
                                 "QPushButton:hover {"
                                    "color:black;} "
                                 );
    QHBoxLayout * contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(2,2,2,2);
    contentLayout->setSpacing(4);

    fileTypeLabel = new QLabel(contentWidget);
    fileTypeLabel->setStyleSheet("border-image: url("
                                 ":/icon/resource/icon/Common_File.png);");
    fileTypeLabel->setFixedSize(ITEM_MAX_HEIGHT-15,ITEM_MAX_HEIGHT-10);

    transferTypeLabel = new QLabel(contentWidget);
    transferTypeLabel->setStyleSheet("border-image: url("
                                            ":/icon/resource/icon/File_Send.png);");
    transferTypeLabel->setFixedSize(20,20);
    fileNameLabel = new QLabel(contentWidget);
    fileNameLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    fileSizeLabel= new QLabel(contentWidget);
    QHBoxLayout * infoLayout = new QHBoxLayout;
    infoLayout->setContentsMargins(0,0,0,0);
    infoLayout->setSpacing(0);
    infoLayout->addWidget(transferTypeLabel);
    infoLayout->addWidget(fileNameLabel);
    infoLayout->addWidget(fileSizeLabel);
    infoLayout->addStretch();

    transferProgress = new QProgressBar(contentWidget);
    transferProgress->setTextVisible(false);
    transferProgress->setOrientation(Qt::Horizontal);
    transferProgress->setFixedHeight(5);
    transferProgress->setMinimum(0);

    curTimeLabel = new QLabel(contentWidget);
    curTimeLabel->setStyleSheet("QLabel {"
                                    "color:lightgray;}"
                                );

    toOffLineBtn = new QPushButton(contentWidget);
    toOffLineBtn->setObjectName("OffLineSend");
    toOffLineBtn->setText(QObject::tr("To off-line send"));
    QObject::connect(toOffLineBtn,SIGNAL(clicked()),q_ptr,SLOT(operateFile()));

    receiveBtn = new QPushButton(contentWidget);
    receiveBtn->setObjectName("RecviveFile");
    receiveBtn->setText(QObject::tr("Recvive"));
    QObject::connect(receiveBtn,SIGNAL(clicked()),q_ptr,SLOT(operateFile()));

    saveAsBtn = new QPushButton(contentWidget);
    saveAsBtn->setObjectName("SaveAs");
    saveAsBtn->setText(QObject::tr("Save as"));
    QObject::connect(saveAsBtn,SIGNAL(clicked()),q_ptr,SLOT(operateFile()));

    cancelBtn = new QPushButton(contentWidget);
    cancelBtn->setObjectName("Cancel");
    cancelBtn->setText(QObject::tr("Cancel"));
    QObject::connect(cancelBtn,SIGNAL(clicked()),q_ptr,SLOT(operateFile()));

    QHBoxLayout * operateLayout = new QHBoxLayout;
    operateLayout->setContentsMargins(0,0,0,0);
    operateLayout->setSpacing(8);
    operateLayout->addWidget(curTimeLabel);
    operateLayout->addStretch(1);
    operateLayout->addWidget(toOffLineBtn);
    operateLayout->addWidget(receiveBtn);
    operateLayout->addWidget(saveAsBtn);
    operateLayout->addWidget(cancelBtn);

    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setSpacing(4);
    leftLayout->addLayout(infoLayout);
    leftLayout->addWidget(transferProgress);
    leftLayout->addLayout(operateLayout);

    contentLayout->addWidget(fileTypeLabel);
    contentLayout->addLayout(leftLayout);
    contentWidget->setLayout(contentLayout);
//    contentLayout->setStretchFactor(leftLayout,1);
//    contentLayout->setStretchFactor(contentLayout,5);

    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

}

TransferFileItem::TransferFileItem(QWidget *parent) :
    QWidget(parent),
    d_ptr(new TransferFileItemPrivate(this))
{
    setFixedHeight(ITEM_MAX_HEIGHT);
}

TransferFileItem::~TransferFileItem()
{

}

/*!
 * @brief 获取传输的文件类型
 * @return 传输的文件类型
 */
TransferFileItem::FileType TransferFileItem::fileType() const
{
    MQ_D(TransferFileItem);

    return (FileType)d->m_fileType;
}

/*!
 * @brief 设置传输的文件类型
 * @param type 传输的文件类型
 */
void TransferFileItem::setFileType(TransferFileItem::FileType type)
{
    MQ_D(TransferFileItem);

    d->m_fileType = type;
    //TODO LYS-20180711 根据文件类型设置fileTypeLabel背景样式
    switch (type) {
    case COMMONFILE:
        d->fileTypeLabel->setStyleSheet("border-image: url("
                                        ":/icon/resource/icon/Common_File.png);");
        break;
    case COMPRESSFILE:
        d->fileTypeLabel->setStyleSheet("border-image: url("
                                        ":/icon/resource/icon/Compress_File.png);");
        break;
    case FOLDERFILE:
        d->fileTypeLabel->setStyleSheet("border-image: url("
                                        ":/icon/resource/icon/Folder_File.png);");
        break;
    default:
        break;
    }
}

/*!
 * @brief 获取文件传输类型
 * @return 文件传输类型
 */
TransferFileItem::TransferType TransferFileItem::transferType() const
{
    MQ_D(TransferFileItem);

    return (TransferType)d->m_transferType;
}

/*!
 * @brief 设置文件传输类型
 * @param type 文件传输类型
 */
void TransferFileItem::setTransferType(TransferFileItem::TransferType type)
{
    MQ_D(TransferFileItem);

    d->m_transferType = type;
    switch (type) {
    case SENDFile:
        d->curTimeLabel->setVisible(false);
        if(d->m_fileType == FOLDERFILE)
        {
            d->toOffLineBtn->setVisible(false);
        }
        else
        {
            d->toOffLineBtn->setVisible(true);
        }
        d->receiveBtn->setVisible(false);
        d->saveAsBtn->setVisible(false);
        d->cancelBtn->setVisible(true);
        d->transferTypeLabel->setStyleSheet("border-image: url("
                                        ":/icon/resource/icon/File_Send.png);");
        break;
    case RECVFile:
        d->curTimeLabel->setVisible(true);
        d->toOffLineBtn->setVisible(false);
        d->receiveBtn->setVisible(true);
        d->saveAsBtn->setVisible(true);
        d->cancelBtn->setVisible(true);
        d->transferTypeLabel->setStyleSheet("border-image: url("
                                        ":/icon/resource/icon/File_Recv.png);");
        break;
    default:
        break;
    }
}

/*!
 * @brief 获取当前传输的文件名
 * @return 当前传输的文件名
 */
QString TransferFileItem::fileName() const
{
    MQ_D(TransferFileItem);

    return d->fileNameLabel->text();;
}

/*!
 * @brief 设置当前传输的文件名
 * @param file 当前传输的文件名
 */
void TransferFileItem::setFileName(const QString &file)
{
    MQ_D(TransferFileItem);

    d->fileNameLabel->setText(file);
}

/*!
 * @brief 获取传输的文件大小（单位为B，字节）
 * @return 传输的文件大小（单位为B，字节）
 */
int TransferFileItem::fileSize() const
{
    MQ_D(TransferFileItem);

    return d->m_totalSize;
}

/*!
 * @brief 设置传输的文件大小
 * @param size 传输的文件大小（单位为B，字节）
 */
void TransferFileItem::setFileSize(const int size)
{
    MQ_D(TransferFileItem);

    d->m_totalSize = size;
    QString t_fileSize = QString();
    if(size < 1024)
    {
        QString t_showSize = QString::number(size);
        t_fileSize = QString("(%1B)").arg(t_showSize);
    }
    else if(size > 1024)
    {
        QString t_showSize = QString::number(size/1024);
        t_fileSize = QString("(%1KB)").arg(t_showSize);
    }
    else if(size > 1024*1024)
    {
        QString t_showSize = QString::number(size/(1024*1024));
        t_fileSize = QString("(%1MB)").arg(t_showSize);
    }
    d->fileSizeLabel->setText(t_fileSize);
    d->transferProgress->setMaximum(size);
}

/*!
 * @brief 获取文件接收时间（接收文件模式下）
 * @return 文件接收时间
 */
QString TransferFileItem::recvTime() const
{
    MQ_D(TransferFileItem);

    if(d->m_transferType == RECVFile)
    {
        return d->curTimeLabel->text();
    }
    else
    {
        return QString();
    }
}

/*!
 * @brief 设置文件接收时间（接收文件模式下）
 * @param time 文件接收时间
 */
void TransferFileItem::setRecvTime(const QString &time)
{
    MQ_D(TransferFileItem);

    if(d->m_transferType == RECVFile)
    {
        d->curTimeLabel->setText(time);
    }
}

/*!
 * @brief 获取文件已传输大小
 * @return 文件已传输大小（单位为B，字节）
 */
int TransferFileItem::finishedSize() const
{
    MQ_D(TransferFileItem);

    return d->m_finishedSize;
}

/*!
 * @brief 设置文件已传输大小
 * @param size 文件已传输大小（单位为B，字节）
 */
void TransferFileItem::setFinishedSize(const int size)
{
    MQ_D(TransferFileItem);

    d->m_finishedSize = size;
    d->transferProgress->setValue(size);
}

/*!
 * @brief 响应文件传输操作(离线发送、接收、另存为、取消)
 */
void TransferFileItem::operateFile()
{
    MQ_D(TransferFileItem);

    QString t_objName = QObject::sender()->objectName();
    if(t_objName == d->toOffLineBtn->objectName())
    {
        emit toOffLineSend(this);
    }
    else if(t_objName == d->receiveBtn->objectName())
    {
        emit startRecvFile(this);
    }
    else if(t_objName == d->saveAsBtn->objectName())
    {
        emit saveAsFile(this);
    }
    else if(t_objName == d->cancelBtn->objectName())
    {
        emit cancelTransfer(this);
    }
}

/*!
 * @brief 设置文件传输用户信息
 * @param desc 文件描述信息
 */
void TransferFileItem::setSenderFileDesc(const SenderFileDesc &desc)
{
    MQ_D(TransferFileItem);
    d->m_fileDesc = desc;
}

/*!
 * @brief 获取当前文件传输描述信息
 * @return 文件描述信息
 */
SenderFileDesc TransferFileItem::senderFileDesc()
{
    MQ_D(TransferFileItem);
    return d->m_fileDesc;
}

/*!
 * @brief 实时更新界面状态
 * @param progress 文件传输进度控制信息结构体
 */
void TransferFileItem::slot_SetTransStatus(FileTransProgress progress)
{
    if(progress.fileFullPath != fileName())
    {
        return ;
    }

    setFileSize(progress.totleBytes);
    setFinishedSize(progress.readySendBytes);
}
