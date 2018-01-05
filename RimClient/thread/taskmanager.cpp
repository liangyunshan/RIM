#include "taskmanager.h"

#include <QWriteLocker>

#include "Util/rlog.h"
#include "rtask.h"

TaskManager::TaskManager(QObject *parent) : QObject(parent)
{

}

/*!
     * @brief 添加新服务，若服务未开启，会自动开启此服务
     * @param[in] toolButton 待插入的工具按钮
     * @return 是否插入成功
     */
void TaskManager::addTask(RTask *task)
{
    if(task == Q_NULLPTR)
    {
        LOG_ERROR("Task is null~");
        return;
    }

    QWriteLocker locker(&lock);
    if(task->running())
    {
        task->startMe();
    }
    tasks.append(task);
}

void TaskManager::removeTask(RTask *task)
{
    if(task == Q_NULLPTR)
    {
        LOG_ERROR("Task is null~");
        return;
    }

    QWriteLocker locker(&lock);

    for(int i = 0 ; i < tasks.size(); i++)
    {
        if(tasks.at(i) == task)
        {
            if(task->running())
            {
                task->stopMe();
            }
            tasks.removeAt(i);
            break;
        }
    }
}

void TaskManager::removeAll()
{
    QWriteLocker locker(&lock);

    for(int i = 0 ; i < tasks.size(); i++)
    {
        if(tasks.at(i)->running())
        {
            tasks.at(i)->stopMe();
        }
    }

    tasks.clear();
}
