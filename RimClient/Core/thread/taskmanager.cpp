#include "taskmanager.h"

#include "Util/rlog.h"
#include "Network/rtask.h"
#include "thread/msgreceiveproctask.h"
#include "thread/filereceiveproctask.h"
#include "thread/filerecvtask.h"
#include "thread/historyrecordtask.h"
#include "thread/file716sendtask.h"
#include "Network/netconnector.h"

TaskManager::TaskManager(QObject *parent) : QObject(parent)
{

}

void TaskManager::initTask()
{
    if(tasks.size() > 0 )
        removeAll();

    //文本线程
    addTask(TEXT_NET_CONC,shared_ptr<ClientNetwork::RTask>(new TextNetConnector()));
    addTask(MSG_RECV_PROC,shared_ptr<ClientNetwork::RTask>(new MsgReceiveProcTask()));

    //文件线程
    addTask(FILE_NET_CONC,shared_ptr<ClientNetwork::RTask>(new FileNetConnector()));
    addTask(FILE_SEND_PROC,shared_ptr<ClientNetwork::RTask>(new File716SendTask()));
#ifndef __LOCAL_CONTACT__
    addTask(FILE_RECV_PROC,shared_ptr<ClientNetwork::RTask>(new FileReceiveProcTask()));
    addTask(FILE_RECV,shared_ptr<ClientNetwork::RTask>(new FileRecvTask()));
#endif
    addTask(HISTORY_RECORD,shared_ptr<ClientNetwork::RTask>(new HistoryRecordTask()));
}

/*!
 * @brief 添加新服务，若服务未开启，会自动开启此服务
 * @param[in] toolButton 待插入的工具按钮
 */
void TaskManager::addTask(TaskType type,shared_ptr<ClientNetwork::RTask> task)
{
    if(task.get() == Q_NULLPTR)
    {
        RLOG_ERROR("Task is null~");
        return;
    }

    unique_lock<mutex> ul(containMutex);

    if(task.get() && !task->running())
        task->startMe();

    tasks.insert(pair<TaskType,shared_ptr<ClientNetwork::RTask>>(type,task));
}

void TaskManager::removeAll()
{
    unique_lock<mutex> ul(containMutex);

    auto func = [](pair<TaskType,shared_ptr<ClientNetwork::RTask>> item){
        if(item.second.get() && item.second.get()->running()){
            item.second.reset();
        }
    };
    for_each(func);

    tasks.clear();
}

bool TaskManager::isReseted()
{
    unique_lock<mutex> ul(containMutex);
    return tasks.size();
}

shared_ptr<ClientNetwork::RTask> TaskManager::operator[](TaskManager::TaskType t)
{
    return task(t);
}

shared_ptr<ClientNetwork::RTask> TaskManager::task(TaskManager::TaskType t)
{
    unique_lock<mutex>(containMutex);
    if(tasks.find(t) != tasks.end())
        return (*tasks.find(t)).second;
    return shared_ptr<ClientNetwork::RTask>(nullptr);
}

void TaskManager::for_each(function<void(const pair<TaskType,shared_ptr<ClientNetwork::RTask>> &)> func)
{
    ::for_each(tasks.begin(),tasks.end(),func);
}
