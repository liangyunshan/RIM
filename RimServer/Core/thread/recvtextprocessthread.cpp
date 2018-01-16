#include "recvtextprocessthread.h"

#include "Network/netglobal.h"
#include "Network/netutils.h"
#include "Network/tcpclient.h"
#include "Util/rlog.h"

#include <QDebug>

#include <WinSock2.h>
#pragma  comment(lib,"ws2_32.lib")

using namespace ServerNetwork;

RecvTextProcessThread::RecvTextProcessThread()
{

}

void RecvTextProcessThread::run()
{
    while(true)
    {
        if(G_RecvButts.size() == 0)
        {
            G_RecvMutex.lock();
            G_RecvCondition.wait(&G_RecvMutex);
            G_RecvMutex.unlock();
        }

        QByteArray data;
        int count = 0;

        G_RecvMutex.lock();

        if(G_RecvButts.size() > 0)
        {
           data =  G_RecvButts.dequeue();
           count = G_RecvButts.size();
        }

        G_RecvMutex.unlock();

        msleep(qrand()%20);

        if(data.size() > 0)
        {
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<data<<data.size()<<count;
        }
    }
}
