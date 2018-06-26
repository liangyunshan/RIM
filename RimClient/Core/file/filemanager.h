/*!
 *  @brief     文件队列管理
 *  @details   提供对上传/下载文件的管理，包括添加上传/下载文件、监控下载文件的状态、移除完成或取消的上传/下载文件
 *  @file      filemanager.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.03.20
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QMap>
#include <QMutex>

#include "../protocol/protocoldata.h"

class FileDesc;

class FileManager
{
public:
    FileManager();

    bool isEmpty();
    int size();

    bool addFile(const ProtocolType::FileItemRequest & desc, const QString savePath);
    bool removeFile(QString fileId);

    bool addUploadFile(FileDesc *desc);

    FileDesc *getFile(const QString fileId);

private:
    QMap<QString,FileDesc *> recvFileMap;
    QMutex mutex;
};

#endif // FILEMANAGER_H
