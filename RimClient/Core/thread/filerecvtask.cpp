#include "filerecvtask.h"

#include <QReadWriteLock>
#include <QFileInfo>
#include <QDebug>
#include <QDataStream>
#include <QMutexLocker>

#include "global.h"
#include "datastruct.h"
#include "Util/rutil.h"
#include "Util/rbuffer.h"
#include "rsingleton.h"
#include "Network/msgwrap.h"
#include "file/filedesc.h"
#include "file/filemanager.h"
#include "user/user.h"

FileRecvTask * FileRecvTask::imageTask = NULL;

FileRecvTask::FileRecvTask()
{
    imageTask = this;
    workMode = FileRequestMode;
    isFileTransfer = false;
    currTransFile = NULL;
}

FileRecvTask::~FileRecvTask()
{
    stopMe();
    wait();
}

FileRecvTask *FileRecvTask::instance()
{
    return imageTask;
}

/*!
 * @brief 添加图片操作请求
 * @param[in] item 请求描述信息
 * @return 是否添加成功
 */
bool FileRecvTask::addSendItem(FileItemDesc *item)
{
    if(item == NULL)
    {
        return false;
    }

    mutex.lock();
    sendItems.enqueue(item);
    mutex.unlock();

    if(!isFileTransfer)
    {
        workMode = FileRequestMode;
        waitCondition.wakeOne();
    }

    return true;
}

bool FileRecvTask::addRecvItem(SimpleFileItemRequest *item)
{
    recvItems.append(item);

    RSingleton<MsgWrap>::instance()->handelFileControl(item);

    return true;
}

void FileRecvTask::sendControlItem(SimpleFileItemRequest *item)
{
    RSingleton<MsgWrap>::instance()->handelFileControl(item);
}

void FileRecvTask::startMe()
{
    RTask::startMe();
    runningFlag = true;
    if(!isRunning())
    {
        start();
    }
    else
    {
        waitCondition.wakeOne();
    }
}

void FileRecvTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    waitCondition.wakeOne();
}

bool FileRecvTask::containsTask(QString fileId)
{
//    QMutexLocker lock(&mutex);
//    QQueue<FileItemDesc *>::iterator iter = sendItems.begin();
//    while(iter != sendItems.end())
//    {
//        if((*iter)->md5 == md5)
//        {
//            return true;
//        }
//    }
    if(currTransFileId == fileId)
    {
        return true;
    }
    return false;
}

void FileRecvTask::transfer(QString fileId)
{
    workMode = FileTransMode;
    waitCondition.wakeOne();
}

void FileRecvTask::nextFile()
{
    if(!isFileTransfer)
    {
        workMode = FileRequestMode;
        waitCondition.wakeOne();
    }
}

void FileRecvTask::run()
{
    while(runningFlag)
    {
        switch(workMode)
        {
            case FileRequestMode:
                {
                    while(runningFlag && sendItems.isEmpty())
                    {
                        mutex.lock();
                        waitCondition.wait(&mutex);
                        mutex.unlock();
                    }

                    if(runningFlag && sendItems.size() > 0)
                    {
                        if(currTransFile != NULL)
                        {
                            delete currTransFile;
                            currTransFile = NULL;
                        }
                        mutex.lock();
                        currTransFile = sendItems.dequeue();
                        mutex.unlock();

                        handleItem();

                        mutex.lock();
                        waitCondition.wait(&mutex);
                        mutex.unlock();
                    }
                }
                break;
            case FileTransMode:
                {
                    transferFile();
                    mutex.lock();
                    waitCondition.wait(&mutex);
                    mutex.unlock();
                }
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief 向服务器发送文件传输请求
 * @details 服务器会在将请求绑定至此socket对应的文件队列
 */
void FileRecvTask::handleItem()
{
    if(currTransFile == NULL)
    {
        return;
    }

    FileItemRequest * fileRequest = new FileItemRequest;
    fileRequest->itemType = currTransFile->itemType;
    fileRequest->fileName = QFileInfo(currTransFile->fullPath).fileName();
    fileRequest->size = currTransFile->fileSize;
    fileRequest->fileId = RUtil::UUID();
    fileRequest->md5 = RUtil::MD5File(currTransFile->fullPath);
    fileRequest->accountId = G_User->BaseInfo().accountId;
    fileRequest->otherId = currTransFile->otherSideId;

    FileDesc * fileDesc = new FileDesc;
    fileDesc->itemType = static_cast<int>(fileRequest->itemType);
    fileDesc->size = fileRequest->size;
    fileDesc->fileName = fileRequest->fileName;
    fileDesc->md5 = fileRequest->md5;
    fileDesc->fileId = fileRequest->fileId;
    fileDesc->accountId = fileRequest->accountId;
    fileDesc->otherId = fileRequest->otherId;
    fileDesc->filePath = currTransFile->fullPath;

    RSingleton<FileManager>::instance()->addUploadFile(fileDesc);

    currTransFileId = fileRequest->fileId;

    RSingleton<MsgWrap>::instance()->handleFileRequest(fileRequest);
}

void FileRecvTask::transferFile()
{
    if(currTransFile != NULL)
    {
        QFile file(currTransFile->fullPath);
        if(!file.exists() || !file.open(QFile::ReadOnly))
        {
            //TODO 提示发送失败
            return;
        }

        isFileTransfer = true;
        int sendLen = 0;
        int currIndex = 0;
        while(!file.atEnd())
        {
            QByteArray data = file.read(900);
            sendLen += data.size();
//            qDebug()<<"SendLen:"<<sendLen<<"_"<<currTransFile->fileSize;
            RSingleton<MsgWrap>::instance()->handleFileData(currTransFileId,currIndex++,data);
        }
        isFileTransfer = false;
    }
}
