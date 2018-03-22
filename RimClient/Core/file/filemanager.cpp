#include "filemanager.h"

#include <QMutexLocker>

#include "filedesc.h"

FileManager::FileManager()
{

}

bool FileManager::isEmpty()
{
    return size() == 0;
}

int FileManager::size()
{
    QMutexLocker locker(&mutex);
    return recvFileMap.size();
}

bool FileManager::addFile(const ProtocolType::FileItemRequest &desc,const QString savePath)
{
    QMutexLocker locker(&mutex);

    if(!recvFileMap.contains(desc.fileId))
    {
        FileRecvDesc * tmpDesc = new FileRecvDesc;
        tmpDesc->itemType = desc.itemType;
        tmpDesc->size = desc.size;
        tmpDesc->writeLen = 0;
        tmpDesc->fileName = desc.fileName;
        tmpDesc->md5 = desc.md5;
        tmpDesc->accountId = desc.accountId;
        tmpDesc->otherId = desc.otherId;
        tmpDesc->saveFilePath = savePath;

        recvFileMap.insert(desc.fileId,tmpDesc);

        return true;
    }

    return false;
}

bool FileManager::removeFile(QString fileId)
{
    QMutexLocker locker(&mutex);
    if(recvFileMap.contains(fileId))
    {
        FileRecvDesc * desc = recvFileMap.take(fileId);
        delete desc;

        return true;
    }

    return false;
}

FileRecvDesc *FileManager::getFile(const QString fileId)
{
    QMutexLocker locker(&mutex);
    if(recvFileMap.contains(fileId))
    {
        return recvFileMap.value(fileId);
    }
    return nullptr;
}
