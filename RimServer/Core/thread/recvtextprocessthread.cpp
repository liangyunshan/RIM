#include "recvtextprocessthread.h"

#include "Network/netglobal.h"
#include "Network/msgparse/msgparsefactory.h"
#include "Network/wraprule/qdb21_wraprule.h"
#include "Network/wraprule/qdb2051_wraprule.h"
#include "Network/head.h"
#include "rsingleton.h"
#include "Network/tcpclient.h"

RecvTextProcessThread::RecvTextProcessThread()
{

}

void RecvTextProcessThread::setDatabase(Database *db)
{
    database = db;
}

void RecvTextProcessThread::run()
{
    while(true)
    {
        while(G_RecvButts.size() == 0)
        {
            G_RecvCondition.wait(std::unique_lock<std::mutex>(G_RecvMutex));
        }

        RecvUnit sockData;

        G_RecvMutex.lock();

        if(G_RecvButts.size() > 0)
        {
           sockData =  G_RecvButts.front();
           G_RecvButts.pop();
        }

        G_RecvMutex.unlock();

#ifdef __LOCAL_CONTACT__
        ProtocolPackage recvPack = RSingleton<QDB21_WrapRule>::instance()->unwrap(sockData.data);
        ProtocolPackage recv2051Pack = RSingleton<QDB2051_WrapRule>::instance()->unwrap(recvPack.data);
        sockData.data = recv2051Pack.data;
        RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(database,sockData);
#else
        RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(database,sockData);
#endif
    }
}
