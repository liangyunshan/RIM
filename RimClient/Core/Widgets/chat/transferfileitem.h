#ifndef TRANSFERFILEITEM_H
#define TRANSFERFILEITEM_H

#include <QWidget>
#include "protocol/datastruct.h"

class TransferFileItemPrivate;

class TransferFileItem : public QWidget
{
    Q_OBJECT
public:
    enum FileType
    {
        COMMONFILE,     //普通文件
        COMPRESSFILE,   //压缩文件
        FOLDERFILE      //文件夹文件
    };
    enum TransferType
    {
        SENDFile,       //发送文件
        RECVFile        //接收文件
    };
    explicit TransferFileItem(QWidget *parent = 0);
    ~TransferFileItem();

    FileType fileType() const;
    void setFileType(FileType type = COMMONFILE);

    TransferType transferType() const;
    void setTransferType(TransferType type = SENDFile);

    QString fileName() const;
    void setFileName(const QString &file);

    int fileSize() const;
    void setFileSize(const int size);

    QString recvTime() const;
    void setRecvTime(const QString &time);

    int finishedSize() const;
    void setFinishedSize(const int size);

    void setSenderFileDesc(const SenderFileDesc &desc);
    SenderFileDesc senderFileDesc();

public slots:
    void slot_SetTransStatus(FileTransProgress);

private:
    TransferFileItemPrivate * d_ptr;
};

#endif // TRANSFERFILEITEM_H
