#include "sendtextprocessthread.h"

#include <QDebug>

#include "Network/netglobal.h"
#include "Network/netutils.h"
#include "Network/tcpclient.h"
#include "Network/iocpcontext.h"
#include "Network/tcpserver.h"
#include "Util/rlog.h"

using namespace ServerNetwork;

SendTextProcessThread::SendTextProcessThread()
{
    runningFlag = false;
}

SendTextProcessThread::~SendTextProcessThread()
{
    wait();
}

void SendTextProcessThread::run()
{
    runningFlag = true;

    while(runningFlag)
    {
        if(G_SendButts.size() == 0)
        {
            G_SendMutex.lock();
            G_SendCondition.wait(&G_SendMutex);
            G_SendMutex.unlock();
        }

        SocketOutData sockData;

        G_SendMutex.lock();

        if(G_SendButts.size() > 0)
        {
           sockData =  G_SendButts.dequeue();
        }

        G_SendMutex.unlock();

        TcpClient * client = TcpServer::instance()->clientManager()->getClient(sockData.sockId);

        if(client != NULL)
        {
            IocpContext * context = IocpContext::create(IocpType::IOCP_SEND,client);
            memcpy(context->getPakcet(),sockData.data.data(),sockData.data.length());

            context->getWSABUF().len = sockData.data.length();

            DWORD sendFlags = 0;

            if(WSASend(sockData.sockId, &context->getWSABUF(), 1, NULL, sendFlags, &context->getOverLapped(), NULL) == SOCKET_ERROR)
            {
                int error = WSAGetLastError();

                if(error != ERROR_IO_PENDING)
                {

                }
            }
            else
            {
                qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"send Client";
            }
        }

    }
}
