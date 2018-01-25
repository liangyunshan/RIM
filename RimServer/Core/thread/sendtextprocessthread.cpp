#include "sendtextprocessthread.h"

#include <QDebug>
#include <qmath.h>

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
        while(G_SendButts.size() == 0)
        {
            G_SendMutex.lock();
            G_SendCondition.wait(&G_SendMutex);
            G_SendMutex.unlock();
        }

        if(runningFlag)
        {
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
                DataPacket packet;
                memset((char *)&packet,0,sizeof(DataPacket));
                packet.magicNum = SEND_MAGIC_NUM;

                packet.packId = client->getPackId();
                packet.totalIndex = qCeil(((float)sockData.data.length() / MAX_PACKET));
                packet.totalLen = sockData.data.size();

                int sendLen = 0;
                for(unsigned int i = 0; i < packet.totalIndex; i++)
                {
                    packet.currentIndex = i;
                    int leftLen = sockData.data.size() - sendLen;
                    packet.currentLen = leftLen > MAX_PACKET ? MAX_PACKET: leftLen;

                    int dataLen = packet.currentLen + sizeof(DataPacket);

                    IocpContext * context = IocpContext::create(IocpType::IOCP_SEND,client);
                    memcpy(context->getPakcet(),(char *)&packet,sizeof(DataPacket));
                    memcpy(context->getPakcet()+ sizeof(DataPacket),sockData.data.data() + sendLen,packet.currentLen);

                    context->getWSABUF().len = dataLen;

                    DWORD sendFlags = 0;
                    DWORD sendLength = 0;

                    if(WSASend(sockData.sockId, &context->getWSABUF(), 1, &sendLength, sendFlags, &context->getOverLapped(), NULL) == SOCKET_ERROR)
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

                    if(sendLength == dataLen)
                    {
                        sendLen += packet.currentLen;
                    }
                }
            }
        }
    }
}
