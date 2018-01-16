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

class RTask;

class TaskManager : public QObject
{
    Q_OBJECT
public:
    explicit TaskManager(QObject *parent = 0);

    void addTask(RTask * task);

    void removeTask(RTask * task);
    void removeAll();

private:
    QReadWriteLock lock;
    QList<RTask *> tasks;

};

#endif // TASKMANAGER_H
