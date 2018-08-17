#include "workthread.h"

#include <QDebug>
#include <Mswsock.h>
#include <process.h>

#include "connection/tcpclient.h"
#include "netutils.h"
#include "Util/rlog.h"
#include "netglobal.h"
#include "../dataprocess/sockdatahandler.h"

namespace ServerNetwork {

WorkThread::WorkThread(SharedIocpData *data):
    serverSharedData(data)
{
    threadId = (HANDLE)_beginthreadex(nullptr, 0,iocpProc,this, 0, nullptr);

#ifdef __LOCAL_CONTACT__
    dataPacketRule = std::make_shared<TCP495DataPacketRule>();
#else
    dataPacketRule = std::make_shared<TCPDataPacketRule>();
#endif
    Handler * handler = new SockTextDataHandler();
    dataPacketRule->registDataHandler(handler);
}

unsigned  __stdcall iocpProc(LPVOID v)
{
    WorkThread * thread = (WorkThread *)v;

    unsigned long recvLength;
    TcpClient * recvClient;

    IocpContext * ioData;

    while (true)
    {
        recvLength = 0;
        ioData = NULL;

        int ret = GetQueuedCompletionStatus(thread->serverSharedData->m_ioCompletionPort, &recvLength, (PULONG_PTR)&recvClient, (LPOVERLAPPED*)&ioData, WSA_INFINITE);
        if(ret == 0)
        {
            RLOG_ERROR("Get iocp error!");
            thread->handleClose(ioData);
            continue;
        }

        if(recvLength == 0 && ioData->getType() == IOCP_RECV)
        {
            thread->serverSharedData->m_clientManager->remove(recvClient);
            continue;
        }

        thread->handleIo(ioData,recvLength,recvClient);
    }

    return 0;
}

void WorkThread::handleIo(IocpContext *ioData, unsigned long recvLength, TcpClient *recvClient)
{
    Q_ASSERT(ioData != nullptr);

    switch (ioData->getType())
    {
        case IOCP_RECV:
                        {
                            handleRecv(ioData,recvLength,recvClient);
                        }
                        break;
        case IOCP_ACCPET:
                        {
                            handleAccept(ioData);
                        }
                        break;
        case IOCP_SEND:
                        {
                            handleSend(ioData);
                        }
                        break;
        default:break;
    }
}

void WorkThread::handleRecv(IocpContext *ioData, unsigned long recvLen, TcpClient* tcpClient)
{
    dataPacketRule->bindContext(ioData,recvLen);

    DWORD dwRecv = 0;
    DWORD dwFlags = 0;

    WSARecv(tcpClient->socket(),&(ioData->getWSABUF()), 1, &dwRecv, &dwFlags,&(ioData->getOverLapped()), NULL);
}

void WorkThread::handleAccept(IocpContext *ioData)
{
    int addrSize = (sizeof(struct sockaddr_in) + 16);

    struct sockaddr_in* lockAddr = NULL;
    int lockLen = sizeof(struct sockaddr_in);

    struct sockaddr_in* remoteAddr = NULL;
    int remoteLen = sizeof(struct sockaddr_in);

    //获取连接socket的信息
    GetAcceptExSockaddrs(ioData->getWSABUF().buf,0,addrSize, addrSize,
            (struct sockaddr**)&lockAddr, &lockLen,
                (struct sockaddr**)&remoteAddr, &remoteLen);

    TcpClient * client = ioData->getClient();
    client->setPort(remoteAddr->sin_port);
    client->setIp(inet_ntoa(remoteAddr->sin_addr));

    serverSharedData->m_clientManager->addClient(client);

    NetUtil::crateIocp(client->socket(), serverSharedData, (DWORD)client);

    NetUtil::postRecv(ioData);

    NetUtil::postAccept(serverSharedData);
}

void WorkThread::handleSend(IocpContext *ioData)
{
    if(ioData->getClient() != NULL)
    {
        static int senNum = 0;
        senNum++;
        static int totalLen = 0;
        totalLen+=ioData->getWSABUF().len;
//        qDebug()<<"Send:"<<ioData->getWSABUF().len<<"__"<<totalLen<<"__+_"<<senNum;
        IocpContext::destory(ioData);
    }
}

void WorkThread::handleClose(IocpContext *ioData)
{
    if(ioData->getClient() != NULL)
    {
        qDebug()<<"___delete client___";
        serverSharedData->m_clientManager->remove(ioData->getClient());
        IocpContext::destory(ioData);
    }
}

}  //namespace ServerNetwork
