#include "historyrecordtask.h"

#include <QDebug>
#include "Util/rutil.h"

HistoryRecordTask* HistoryRecordTask::hitoryTask = nullptr;

HistoryRecordTask::HistoryRecordTask(QObject *parent):
    ClientNetwork::RTask(parent)
{
    hitoryTask = this;
}

HistoryRecordTask::~HistoryRecordTask()
{
    hitoryTask = nullptr;
    stopMe();
    wait();
}

HistoryRecordTask *HistoryRecordTask::instance()
{
    return hitoryTask;
}

void HistoryRecordTask::executeTask(std::function<bool ()> func,std::function<void()> callBack)
{
    std::unique_lock<std::mutex> lg(fmutex);
    funcMap.insert(std::make_pair(RUtil::UUID(),std::make_pair(func,callBack)));
    cv.notify_all();
}

void HistoryRecordTask::startMe()
{
    RTask::startMe();

    runningFlag = true;

    if(!isRunning())
        start();
    else
        cv.notify_all();
}

void HistoryRecordTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    cv.notify_all();
}

void HistoryRecordTask::run()
{
    while(runningFlag){

        while(runningFlag && funcMap.size() <= 0){
            std::unique_lock<std::mutex> ul(fmutex);
            cv.wait(ul);
        }

        std::unique_lock<std::mutex> ul(fmutex);
        if(runningFlag && funcMap.size() > 0){

            std::map<QString,std::pair<std::function<bool()>,std::function<void()>>>::iterator it = funcMap.begin();

            while(it != funcMap.end()){
                if(((*it).second.first)()){
                    if((*it).second.second != nullptr)
                        ((*it).second.second)();
                }
                it = funcMap.erase(it);
            }
        }else{
            break;
        }
    }
}
