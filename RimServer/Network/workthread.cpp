#include "workthread.h"

#include <QDebug>
#include <Mswsock.h>
#include <process.h>

#include "tcpclient.h"
#include "netutils.h"
#include "Util/rlog.h"
#include "netglobal.h"

namespace ServerNetwork {

WorkThread::WorkThread(SharedIocpData *data):
    serverSharedData(data)
{
    threadId = (HANDLE)_beginthreadex(nullptr, 0,iocpProc,this, 0, nullptr);
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
    ioData->getPakcet()[recvLen] = 0;

    TestSocket = tcpClient->socket();

    G_RecvMutex.lock();

    QByteArray array;
    array.resize(recvLen);
    memcpy(array.data(),ioData->getPakcet(),recvLen - 1);

    G_RecvButts.enqueue(array);

    G_RecvMutex.unlock();

    G_RecvCondition.wakeOne();

    DWORD dwRecv = 0;
    DWORD dwFlags = 0;

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<ioData->getClient()->socket();

    IocpContext * context = IocpContext::create(IocpType::IOCP_SEND,ioData->getClient());
    memcpy(context->getPakcet(),ioData->getPakcet(),recvLen);

    context->getWSABUF().len = recvLen;

    DWORD sendFlags = 0;

    if(WSASend(context->getClient()->socket(), &context->getWSABUF(), 1, NULL, sendFlags, &context->getOverLapped(), NULL) == SOCKET_ERROR)
    {
        int error = WSAGetLastError();

        if(error != ERROR_IO_PENDING)
        {

        }
    }
    else
    {

    }

    int ret = WSARecv(tcpClient->socket(),&(ioData->getWSABUF()), 1, &dwRecv, &dwFlags,&(ioData->getOverLapped()), NULL);
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

    serverSharedData->m_clientManager->addClient(ioData->getClient());

    NetUtil::crateIocp(ioData->getClient()->socket(), serverSharedData, (DWORD)ioData->getClient());

    NetUtil::postRecv(ioData);

    NetUtil::postAccept(serverSharedData);
}

void WorkThread::handleSend(IocpContext *ioData)
{
    if(ioData->getClient() != NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"delete:"<<ioData->getClient()->socket();
        IocpContext::destory(ioData);
    }
}

void WorkThread::handleClose(IocpContext *ioData)
{
    if(ioData->getClient() != NULL)
    {
        serverSharedData->m_clientManager->remove(ioData->getClient());
        IocpContext::destory(ioData);
    }
}

}   //namespace ServerNetwork
