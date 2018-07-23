/*!
 *  @brief     自定义文件传送item
 *  @details   显示文件传输信息、控制文件传输流程
 *  @file      transferfileitem.h
 *  @author    LYS
 *  @version   1.0.3
 *  @date      2018.07.12
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      1.20180713-LYS 修复文件大小显示错误问题。
 *      2.20180713-LYS 修改文件名显示逻辑。
 *      3.20180716-LYS 添加文件传送状态保存。
 *      4.20180720-LYS 修改文件名label文本显示规则。
 *
 */
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

    float fileSize() const;
    void setFileSize(const float size);

    QString recvTime() const;
    void setRecvTime(const QString &time);

    float finishedSize() const;
    void setFinishedSize(const float size);

    QString taskSerialNo() const;
    void setTaskSerialNo(const QString &seriNo);

    bool taskIfStarted() const;
    void setTaskIfStarted(bool);

signals:
    void saveAsFile(TransferFileItem *);
    void toOffLineSend(TransferFileItem *);
    void startRecvFile(TransferFileItem *);
    void cancelTransfer(TransferFileItem *);

public slots:

private slots:
    void operateFile();

private:
    TransferFileItemPrivate * d_ptr;
};

#endif // TRANSFERFILEITEM_H
