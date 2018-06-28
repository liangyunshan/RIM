#include "filereceiveproctask.h"

#include "Network/netglobal.h"
#include "global.h"

#include "rsingleton.h"
#include "Network/msgparse/binaryparsefactory.h"

FileReceiveProcTask::FileReceiveProcTask(QObject *parent):
    RTask(parent)
{

}

FileReceiveProcTask::~FileReceiveProcTask()
{
    stopMe();
    wait();
}

void FileReceiveProcTask::startMe()
{
    RTask::startMe();

    runningFlag = true;

    if(!isRunning())
    {
        start();
    }
    else
    {
        G_FileRecvCondition.notify_one();
    }
}

void FileReceiveProcTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    G_FileRecvCondition.notify_one();
}

void FileReceiveProcTask::run()
{
    while(runningFlag)
    {
        while(runningFlag && G_FileRecvBuffs.empty())
        {
            std::unique_lock<std::mutex> ul(G_FileRecvMutex);
            G_FileRecvCondition.wait(ul);
        }

        if(runningFlag && G_FileRecvBuffs.size() > 0)
        {
            G_FileRecvMutex.lock();
            RecvUnit array = G_FileRecvBuffs.front();
            G_FileRecvBuffs.pop();
            G_FileRecvMutex.unlock();

            if(array.data.size() > 0)
            {
                ProtocolPackage packData;
                packData.data = array.data;
                RSingleton<BinaryParseFactory>::instance()->getDataParse()->processData(packData);
            }
        }
    }
}
