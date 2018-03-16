#include "imagetask.h"

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

ImageTask * ImageTask::imageTask = NULL;

ImageTask::ImageTask()
{
    imageTask = this;
    workMode = FileRequestMode;
}

ImageTask *ImageTask::instance()
{
    return imageTask;
}

/*!
 * @brief 添加图片操作请求
 * @param[in] item 请求描述信息
 * @return 是否添加成功
 */
bool ImageTask::addItem(FileItemDesc *item)
{
    if(item == NULL)
    {
        return false;
    }

    mutex.lock();
    processItems.enqueue(item);
    mutex.unlock();

    waitCondition.wakeOne();

    return true;
}

void ImageTask::startMe()
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

void ImageTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    waitCondition.wakeOne();
}

bool ImageTask::containsTask(QString md5)
{
//    QMutexLocker lock(&mutex);
//    QQueue<FileItemDesc *>::iterator iter = processItems.begin();
//    while(iter != processItems.end())
//    {
//        if((*iter)->md5 == md5)
//        {
//            return true;
//        }
//    }
    if(currTransFileId == md5)
    {
        return true;
    }
    return false;
}

void ImageTask::transfer(QString fileId)
{
    workMode = FileTransMode;
    waitCondition.wakeOne();
}

void ImageTask::run()
{
    while(runningFlag)
    {
        switch(workMode)
        {
            case FileRequestMode:
                {
                    while(processItems.isEmpty())
                    {
                        mutex.lock();
                        waitCondition.wait(&mutex);
                        mutex.unlock();
                    }

                    if(runningFlag && processItems.size() > 0)
                    {
                        currTransFile = NULL;
                        mutex.lock();
                        currTransFile = processItems.dequeue();
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
 * @brief 处理网络传输请求
 * @param[in] request 请求描述信息
 * @return 无
 */
void ImageTask::handleItem()
{
    if(currTransFile == NULL)
    {
        return;
    }

    FileItemRequest * fileRequest = new FileItemRequest;
    fileRequest->itemType = currTransFile->itemType;
    fileRequest->fileName = QFileInfo(currTransFile->fullPath).fileName();
    fileRequest->size = currTransFile->fileSize;
    fileRequest->md5 = RUtil::MD5File(currTransFile->fullPath);
    fileRequest->accountId = G_UserBaseInfo.accountId;
    fileRequest->otherId = currTransFile->otherSideId;

    currTransFileId = fileRequest->md5;

    RSingleton<MsgWrap>::instance()->handleFileRequest(fileRequest);
}

void ImageTask::transferFile()
{
    if(currTransFile != NULL)
    {
        QFile file(currTransFile->fullPath);
        if(!file.exists())
        {
            //TODO 提示发送失败
            return;
        }

        if(!file.open(QFile::ReadOnly))
        {
            //TODO 提示发送失败
            return;
        }

        int sendLen = 0;
        int currIndex = 0;
        while(!file.atEnd())
        {
            QByteArray data = file.read(900);
            sendLen += data.size();
            qDebug()<<"SendLen:"<<sendLen<<"_"<<currTransFile->fileSize;
            RSingleton<MsgWrap>::instance()->handleFileData(currTransFileId,currIndex++,data);
        }
    }
}
