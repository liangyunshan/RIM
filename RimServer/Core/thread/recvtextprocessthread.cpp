#include "recvtextprocessthread.h"

#include "Network/netglobal.h"
#include "Network/win32net/netutils.h"
#include "Network/tcpclient.h"
#include "Util/rlog.h"
#include "dataparse.h"
#include "rsingleton.h"

#include <QDebug>


using namespace ServerNetwork;

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
            G_RecvMutex.lock();
            G_RecvCondition.wait(&G_RecvMutex);
            G_RecvMutex.unlock();
        }

        SocketInData sockData;
        int count = 0;

        G_RecvMutex.lock();

        if(G_RecvButts.size() > 0)
        {
           sockData =  G_RecvButts.dequeue();
           count = G_RecvButts.size();
        }

        G_RecvMutex.unlock();
        RSingleton<DataParse>::instance()->processData(database,sockData);
    }
}
