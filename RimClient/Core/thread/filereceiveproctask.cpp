#include "filereceiveproctask.h"

#include "Network/netglobal.h"
#include "global.h"

#include "util/rsingleton.h"
#include "Network/msgparse/msgparsefactory.h"
#include "../Network/wraprule/tcp_wraprule.h"

#include <QDebug>

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
    ProtocolPackage package;
#ifdef __LOCAL_CONTACT__

    bool result = false;
    switch(data.extendData.method){
        case C_TCP:
            result = RSingleton<ClientNetwork::TCP_WrapRule>::instance()->unwrap(data.data,package);
            break;
        default:
            break;
    }

    if(result){
        package.bPackType = (unsigned char)data.extendData.type495;
        package.bPeserve = data.extendData.bPeserve;
        package.usSerialNo = data.extendData.usSerialNo;
        package.usOrderNo = data.extendData.usOrderNo;
        package.wOffset = data.extendData.wOffset;
        package.dwPackAllLen = data.extendData.dwPackAllLen;

        RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(package);
    }

#else
    package.data = data.data;
    RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(package);
#endif

}
