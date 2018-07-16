#include "filemanager.h"

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
    std::unique_lock<std::mutex> ul(mutex);
    return recvFileMap.size();
}

bool FileManager::addFile(const ProtocolType::FileItemRequest &desc,const QString savePath)
{
    std::unique_lock<std::mutex> ul(mutex);

    if(recvFileMap.find(desc.fileId) == recvFileMap.end())
    {
        std::shared_ptr<FileDesc> tmpDesc = std::shared_ptr<FileDesc>();
        tmpDesc->itemType = desc.itemType;
        tmpDesc->itemKind = desc.itemKind;
        tmpDesc->size = desc.size;
        tmpDesc->writeLen = 0;
        tmpDesc->fileName = desc.fileName;
        tmpDesc->md5 = desc.md5;
        tmpDesc->fileId = desc.fileId;
        tmpDesc->accountId = desc.accountId;
        tmpDesc->otherId = desc.otherId;
        tmpDesc->filePath = savePath;

        recvFileMap.insert(std::pair<QString,std::shared_ptr<FileDesc>>(desc.fileId,tmpDesc));

        return true;
    }

    return false;
}

bool FileManager::removeFile(QString fileId)
{
    std::unique_lock<std::mutex> ul(mutex);

    auto findIndex = recvFileMap.find(fileId);

    if(findIndex != recvFileMap.end())
    {
        (*findIndex).second.reset();
        recvFileMap.erase(findIndex);
        return true;
    }

    return false;
}

bool FileManager::addUploadFile(std::shared_ptr<FileDesc> desc)
{
    std::unique_lock<std::mutex> ul(mutex);

    if(recvFileMap.find(desc->fileId) == recvFileMap.end())
    {
        recvFileMap.insert(std::pair<QString,std::shared_ptr<FileDesc>>(desc->fileId,desc));
        return true;
    }

    return false;
}

bool FileManager::addFile(std::shared_ptr<FileDesc> &fptr)
{
    std::unique_lock<std::mutex> ul(mutex);

#ifdef __LOCAL_CONTACT__
    auto findIndex = std::find_if(recvFileMap.begin(),recvFileMap.end(),[&](const std::pair<QString,std::shared_ptr<FileDesc>> & ptr){
        return (fptr->accountId == ptr.second->accountId && fptr->usSerialNo == ptr.second->usSerialNo
                    && fptr->fileName == ptr.second->fileName);
    });
#else
    auto findIndex = std::find_if(recvFileMap.begin(),recvFileMap.end(),[&](const std::pair<QString,std::shared_ptr<FileDesc>> & ptr){
        return (fptr->fileId == ptr.second->fileId);
    });
#endif

    if(findIndex == recvFileMap.end()){
        recvFileMap.insert(std::pair<QString,std::shared_ptr<FileDesc>>(fptr->fileId,fptr));
        return true;
    }

    return false;
}

std::shared_ptr<FileDesc> FileManager::getFile(const QString fileId)
{
    std::unique_lock<std::mutex> ul(mutex);

    auto findIndex = recvFileMap.find(fileId);
    if(findIndex != recvFileMap.end())
    {
        return (*findIndex).second;
    }
    return nullptr;
}

#ifdef __LOCAL_CONTACT__
/*!
 * @brief 根据发送方的基本信息查找是否为新文件
 * @param[in] nodeId 发送方节点号
 * @param[in] serialNo 发送流水号
 * @param[in] fileName 发送文件名
 */
std::shared_ptr<FileDesc> FileManager::get716File(const QString &nodeId, const unsigned short &serialNo, const QString &fileName)
{
    std::unique_lock<std::mutex> ul(mutex);

    auto findIndex = std::find_if(recvFileMap.begin(),recvFileMap.end(),[&](const std::pair<QString,std::shared_ptr<FileDesc>>& fptr){
        return (fptr.second->accountId == nodeId && fptr.second->usSerialNo == serialNo
                && fptr.second->fileName == fileName);
    });

    if(findIndex != recvFileMap.end()){
        return (*findIndex).second;
    }

    return nullptr;
}
#endif
