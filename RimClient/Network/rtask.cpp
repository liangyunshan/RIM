#include "rtask.h"

namespace ClientNetwork {

RTask::RTask(QObject *parent):
    taskState(IdleState),
    QThread(parent)
{
    runningFlag = false;
}

RTask::~RTask()
{

}

bool RTask::running()
{
    if(taskState == StartState)
    {
        return true;
    }
    return false;
}

void RTask::startMe()
{
    taskState = StartState;
    emit stateChanged(taskState);
}

void RTask::stopMe()
{
    taskState = StoppedState;
    emit stateChanged(taskState);
}

} //namespace ClientNetwork
