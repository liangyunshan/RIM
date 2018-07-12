#include "msgreceiveproctask.h"

#include "network/netglobal.h"
#include "util/rsingleton.h"
#include "head.h"
#include "jsonkey.h"
#include "messdiapatch.h"
#include "Network/msgparse/msgparsefactory.h"

#include "../Network/wraprule/tcp_wraprule.h"
#include "network/wraprule/wraprule.h"

MsgReceiveProcTask::MsgReceiveProcTask(QObject *parent):
    ClientNetwork::RTask(parent)
{

}

MsgReceiveProcTask::~MsgReceiveProcTask()
{
    stopMe();
    wait();
}

void MsgReceiveProcTask::startMe()
{
    RTask::startMe();

    runningFlag = true;

    if(!isRunning())
    {
        start();
    }
    else
    {
        G_TextRecvCondition.notify_one();
    }
}

void MsgReceiveProcTask::run()
{
    while(runningFlag)
    {
        while(runningFlag && G_TextRecvBuffs.empty())
        {
            std::unique_lock<std::mutex> ul(G_TextRecvMutex);
            G_TextRecvCondition.wait(ul);
        }

        if(runningFlag && G_TextRecvBuffs.size() > 0)
        {
            G_TextRecvMutex.lock();
            RecvUnit array = G_TextRecvBuffs.front();
            G_TextRecvBuffs.pop();
            G_TextRecvMutex.unlock();

            if(array.data.size() > 0)
            {
                validateRecvData(array);
            }
        }
    }
}

void MsgReceiveProcTask::validateRecvData(const RecvUnit &data)
{
    ProtocolPackage packData;
#ifdef __LOCAL_CONTACT__
    RSingleton<ClientNetwork::TCP_WrapRule>::instance()->unwrap(data.data,packData);
    packData.bPackType = (unsigned char)data.extendData.type495;

    bool result = false;
    switch(data.extendData.method){
        case C_TCP:
            result = RSingleton<ClientNetwork::TCP_WrapRule>::instance()->unwrap(data.data,packData);
            break;
        default:
            break;
    }

    if(result)
    {
        RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(packData);
    }
#else
    packData.data = data.data;
    RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(packData);
#endif

}

void MsgReceiveProcTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    G_TextRecvCondition.notify_one();
}
