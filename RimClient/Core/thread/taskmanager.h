/*!
 *  @brief     任务管理
 *  @details   支持添加/插入任务、暂停任务等
 *  @file      taskmanager.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.05
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QList>
#include <QReadWriteLock>
#include <memory>
#include <mutex>
#include <map>

using namespace std;

namespace ClientNetwork {
class RTask;
}

class TaskManager : public QObject
{
    Q_OBJECT
public:
    explicit TaskManager(QObject *parent = 0);

    enum TaskType{
       TEXT_NET_CONC = 0x01,    /*!< 文本网络连接 */
       FILE_NET_CONC,           /*!< 文件网络连接 */
       MSG_RECV_PROC,           /*!< 文本接收线程 */
       FILE_RECV_PROC,          /*!< 文件接收线程*/
       FILE_RECV,               /*!< 文件发送处理队列线程 */
       HISTORY_RECORD           /*!< 历史聊天记录 */
    };

    void initTask();
    void addTask(TaskType type, shared_ptr<ClientNetwork::RTask> task);
    void removeAll();

    bool isReseted();

    shared_ptr<ClientNetwork::RTask> operator[](TaskType t);
    shared_ptr<ClientNetwork::RTask> task(TaskType t);

private:
    void for_each(function<void(const pair<TaskType,shared_ptr<ClientNetwork::RTask>> &)> func);

private:
    mutex containMutex;
    map<TaskType,shared_ptr<ClientNetwork::RTask>> tasks;

};

#endif // TASKMANAGER_H
