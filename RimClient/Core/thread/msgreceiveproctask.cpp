#include "msgreceiveproctask.h"

#include <QJsonDocument>

#include "network/netglobal.h"
#include "rsingleton.h"
#include "head.h"
#include "jsonkey.h"
#include "messdiapatch.h"
#include "Network/msgparse/msgparsefactory.h"

#include "../Network/wraprule/qdb21_wraprule.h"
#include "../Network/wraprule/qdb2051_wraprule.h"

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
            QByteArray array = G_TextRecvBuffs.front();
            G_TextRecvBuffs.pop();
            G_TextRecvMutex.unlock();

            if(array.size() > 0)
            {
                validateRecvData(array);
            }
        }
    }
}

void MsgReceiveProcTask::validateRecvData(const QByteArray &data)
{
#ifdef __LOCAL_CONTACT__
    //716_TK兼容调试,解析头
//    RSingleton<DataProcess>::instance()->proTCPText(RBuffer(data));
    //[~716]
#endif
    RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(data);
}

void MsgReceiveProcTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    G_TextRecvCondition.notify_one();
}
