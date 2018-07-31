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

#include <QString>
#include <memory>
#include <mutex>
#include <map>

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

    bool addUploadFile(std::shared_ptr<FileDesc> desc);

    std::shared_ptr<FileDesc> getFile(const QString fileId);
#ifdef __LOCAL_CONTACT__
    std::shared_ptr<FileDesc> get716File(const QString& fileId);
#endif
    bool addFile(std::shared_ptr<FileDesc> & fptr);

private:
    std::map<QString,std::shared_ptr<FileDesc>> recvFileMap;
    std::mutex mutex;
};

#endif // FILEMANAGER_H
