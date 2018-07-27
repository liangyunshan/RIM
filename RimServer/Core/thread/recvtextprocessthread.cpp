#include "recvtextprocessthread.h"

#include "Network/netglobal.h"
#include "Network/msgparse/msgparsefactory.h"
#include "Network/head.h"
#include "rsingleton.h"
#include "Network/connection/tcpclient.h"

RecvTextProcessThread::RecvTextProcessThread()
{
    runningFlag = true;
}

void RecvTextProcessThread::setDatabase(Database *db)
{
    database = db;
}

void RecvTextProcessThread::run()
{
    while(runningFlag)
    {
        while(G_RecvButts.size() == 0)
        {
            G_RecvCondition.wait(std::unique_lock<std::mutex>(G_RecvMutex));
        }

        if(runningFlag){
            bool existed = false;
            RecvUnit sockData;

            G_RecvMutex.lock();

            if(G_RecvButts.size() > 0)
            {
               sockData =  G_RecvButts.front();
               existed = true;
               G_RecvButts.pop();
            }

            G_RecvMutex.unlock();

            if(existed)
                RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(database,sockData);
        }
    }
}
