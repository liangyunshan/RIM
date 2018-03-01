#include "imagetask.h"

#include <QReadWriteLock>
#include <QDebug>

#include "global.h"
#include "datastruct.h"

ImageTask * ImageTask::imageTask = NULL;

ImageTask::ImageTask()
{
    imageTask = this;
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

void ImageTask::run()
{
    while(runningFlag)
    {
        while(processItems.isEmpty())
        {
            mutex.lock();
            waitCondition.wait(&mutex);
            mutex.unlock();
        }

        if(runningFlag && processItems.size() > 0)
        {
            FileItemDesc * request = NULL;
            mutex.lock();
            request = processItems.dequeue();
            mutex.unlock();

            handleItem(request);
        }
    }
}

/*!
 * @brief 处理图片请求
 * @param[in] request 请求描述信息
 * @return 无
 */
void ImageTask::handleItem(FileItemDesc *request)
{
    if(request == NULL)
    {
        return;
    }

}
