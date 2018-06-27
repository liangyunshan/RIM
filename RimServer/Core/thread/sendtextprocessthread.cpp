#include "sendtextprocessthread.h"

#include <QDebug>
#include <qmath.h>

#include "Network/netglobal.h"
#include "Network/win32net/netutils.h"
#include "Network/tcpclient.h"
#include "Network/win32net/iocpcontext.h"
#include "Network/win32net/tcpserver.h"
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
            if(!runningFlag)
            {
                break;
            }
            G_SendCondition.wait(std::unique_lock<std::mutex>(G_SendMutex));
        }

        if(runningFlag)
        {
            SendUnit sockData;

            G_SendMutex.lock();
            if(G_SendButts.size() > 0)
            {
               sockData =  G_SendButts.front();
               G_SendButts.pop();
            }
            G_SendMutex.unlock();

            TcpClient * client = TcpServer::instance()->clientManager()->getClient(sockData.sockId);
            if(client != NULL)
            {
                DataPacket packet;
                memset((char *)&packet,0,sizeof(DataPacket));
                packet.magicNum = SEND_MAGIC_NUM;

                packet.packId = client->getPackId();
                packet.totalIndex = qCeil(((float)sockData.dataUnit.data.length() / MAX_PACKET));
                packet.totalLen = sockData.dataUnit.data.size();

                int sendLen = 0;
                for(unsigned int i = 0; i < packet.totalIndex; i++)
                {
                    packet.currentIndex = i;
                    int leftLen = sockData.dataUnit.data.size() - sendLen;
                    packet.currentLen = leftLen > MAX_PACKET ? MAX_PACKET: leftLen;

                    int dataLen = packet.currentLen + sizeof(DataPacket);

                    IocpContext * context = IocpContext::create(IocpType::IOCP_SEND,client);
                    memcpy(context->getPakcet(),(char *)&packet,sizeof(DataPacket));
                    memcpy(context->getPakcet()+ sizeof(DataPacket),sockData.dataUnit.data.data() + sendLen,packet.currentLen);

                    context->getWSABUF().len = dataLen;

                    DWORD sendFlags = 0;
                    DWORD sendLength = 0;

                    if(WSASend(sockData.sockId, &context->getWSABUF(), 1, &sendLength, sendFlags, &context->getOverLapped(), NULL) == SOCKET_ERROR)
                    {
                        int error = WSAGetLastError();

                        if(error != ERROR_IO_PENDING)
                        {
                            //TODO 对错误进行处理
                        }
                    }
//                    else
//                    {
//                        qDebug()<<__FILE__<<__FUNCTION__<<"send Client::"<<packet.packId;
//                    }

                    if(sendLength == dataLen)
                    {
                        sendLen += packet.currentLen;
                    }
                }
            }
        }
    }
}
