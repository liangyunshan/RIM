/*!
 *  @brief     处理任务
 *  @details   继承自QThread，根据不同的处理类型，派生出不同的子类
 *  @file      rtask.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.XX
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RTASK_H
#define RTASK_H

#include "network_global.h"
#include <QThread>

namespace ClientNetwork {

class NETWORKSHARED_EXPORT RTask : public QThread
{
    Q_OBJECT
public:
    explicit RTask(QObject * parent = 0);
    ~RTask();

    enum TaskState
    {
        IdleState,              //任务未启动
        StartState,             //任务运行中
        StoppedState            //任务停止
    };

    bool running();

    virtual void startMe();
    virtual void stopMe();

signals:
    void stateChanged(TaskState);

protected:
    TaskState taskState;

    bool runningFlag;
};

} //namespace ClientNetwork

#endif // RTASK_H
