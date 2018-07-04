#include "filereceiveproctask.h"

#include "Network/netglobal.h"
#include "global.h"

#include "rsingleton.h"
#include "Network/msgparse/binaryparsefactory.h"

#include "../Network/wraprule/tcp_wraprule.h"

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
                validateRecvData(array);
            }
        }
    }
}

void FileReceiveProcTask::validateRecvData(const RecvUnit &data)
{
    ProtocolPackage packData;
#ifdef __LOCAL_CONTACT__
    bool result = false;
    switch(data.extendData.method){
        case C_TCP:
        {
            result = RSingleton<TCP_WrapRule>::instance()->unwrap(data.data,packData);
            packData.bPackType = data.extendData.type495;
            packData.bPeserve = data.extendData.bPeserve;;
        }break;
        default:
            break;
    }

    if(result)
    {
        RSingleton<BinaryParseFactory>::instance()->getDataParse()->processData(packData);
    }
#else
    packData.data = data.data;
    RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(packData);
#endif
}
