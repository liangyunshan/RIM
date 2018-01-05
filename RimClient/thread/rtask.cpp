#include "rtask.h"

RTask::RTask(QObject *parent):
    taskState(IdleState),
    QThread(parent)
{

}

RTask::~RTask()
{

}

bool RTask::running()
{
    if(taskState == taskState || taskState == ActiveState)
    {
        return true;
    }
    return false;
}

void RTask::startMe()
{
    taskState = ActiveState;
    emit stateChanged(taskState);
}

void RTask::runMe()
{
    taskState = ActiveState;
    emit stateChanged(taskState);
}

void RTask::stopMe()
{
    taskState = StoppedState;
    emit stateChanged(taskState);
}
