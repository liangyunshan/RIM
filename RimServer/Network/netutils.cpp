#include "netutils.h"

#include <mswsock.h>

#include <QDebug>

#include "Util/rlog.h"

#include "SharedIocpData.h"
#include "tcpserver.h"

namespace ServerNetwork {
namespace NetUtil {

//在接收的socket的ID中为其创建接收缓冲区用于重叠模型中接收数据，当此socket上有数据来时，在IO完成后便可获得通知
void postRecv(IocpContext * ioData)
{
    DWORD dwRecv = 0;
    DWORD dwFlags = 0;
    ioData->setType(IocpType::IOCP_RECV);

    WSARecv(ioData->getClient()->socket(), &(ioData->getWSABUF()),1, &dwRecv, &dwFlags, &(ioData->getOverLapped()), NULL);
}

//异步接收socket连接请求，并将连接的socket信息保存至所创建的client中
void postAccept(SharedIocpData * server)
{
    DWORD dwBytes = 0;
    int addr_size = (sizeof(struct sockaddr_in) + 16);

    TcpClient * client = TcpClient::create();
    IocpContext * context = IocpContext::create(IocpType::IOCP_ACCPET,client);

    AcceptEx(server->m_listenSock.getSocket(), client->socket(), context->getWSABUF().buf, 0,addr_size, addr_size, &dwBytes, &context->getOverLapped());
}

bool crateIocp(SOCKET clientSock,SharedIocpData * sharedData,DWORD iocpKey)
{
    if(CreateIoCompletionPort((HANDLE)clientSock,sharedData->m_ioCompletionPort,iocpKey,0) != sharedData->m_ioCompletionPort)
    {
        RLOG_ERROR("Create iocp error,errcode[%d]",GetLastError());
        return false;
    }

    return true;
}

bool send(SOCKET dest, const char *data, int length)
{
    TcpServer::instance()->clientManager()->getClient(dest);

    IocpContext * context = IocpContext::create(IOCP_ACCPET,NULL);

    DWORD dwBytes = 0;

    if(WSASend(dest,&context->getWSABUF(),1,&dwBytes,0,(LPWSAOVERLAPPED)&context->getOverLapped(),NULL) == SOCKET_ERROR)
    {
        RLOG_INFO("send Error:[%s]",strerror(GetLastError()));
        return false;
    }

    RLOG_INFO("send bytes:[%d]",dwBytes);

    return true;
}


}   //namespace NetUtil
}   //namespace ServerNetwork
