#include "tcpserver.h"

#if defined(Q_OS_WIN)

#include "netglobal.h"
#include "win32net/workthread.h"
#include "win32net/SharedIocpData.h"

#include <process.h>

#include "Util/rlog.h"
#include "connection/tcpclient.h"
#include "win32net/netutils.h"

#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")

namespace ServerNetwork {

TcpServer  * TcpServer::serverInstance = NULL;

TcpServer::TcpServer()
{
    serverInstance = this;

    runningFlag = false;
    dwNumberOfProcessors = 0;

    m_sharedIocpData = new SharedIocpData;
    m_sharedIocpData->m_clientManager = new TcpClientManager;
}

TcpServer *TcpServer::instance()
{
    return serverInstance;
}

TcpClientManager *TcpServer::clientManager()
{
    return m_sharedIocpData->m_clientManager;
}

bool TcpServer::startMe(const char *ip, unsigned short port)
{
   if(ip == NULL || port < 1024)
   {
       RLOG_ERROR("ip or port is invalid!");
       return false;
   }

#ifdef Q_OS_WIN
   if(createListenSocket(ip,port))
   {
        if(createIocpPort())
        {
            if(createWorkThread())
            {
                if(startAccept())
                {

                    return true;
                }
            }
        }
   }
#endif
   return false;
}

bool TcpServer::createListenSocket(const char *ip, unsigned short port)
{
    if(!m_sharedIocpData->m_listenSock.createSocket())
    {
         RLOG_ERROR("Server socket create error!");
         return false;
    }

    if(!m_sharedIocpData->m_listenSock.bind(ip,port))
    {
        RLOG_ERROR("Server socket bind error!");
        return false;
    }

    if(!m_sharedIocpData->m_listenSock.listen())
    {
        RLOG_ERROR("Server socket listen error!");
        return false;
    }

    return true;
}

bool TcpServer::createIocpPort()
{
    m_sharedIocpData->m_ioCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (m_sharedIocpData->m_ioCompletionPort == NULL)
    {
        RLOG_ERROR("Create io somplete port error!");
        m_sharedIocpData->m_listenSock.closeSocket();
        return false;
    }
    return true;
}

bool TcpServer::createWorkThread()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    int maxCount = 2 * info.dwNumberOfProcessors + 1;
    Q_UNUSED(maxCount);
    for(int i = 0; i < 1; i++)
    {
        WorkThread * thread = new WorkThread(m_sharedIocpData);
        workThreads.push_back(thread);
    }

    return true;
}

bool TcpServer::startAccept()
{
    m_sharedIocpData->crateIocpContext();
    HANDLE hiocp = CreateIoCompletionPort((HANDLE)m_sharedIocpData->m_listenSock.getSocket(), m_sharedIocpData->m_ioCompletionPort, (DWORD)0, 0);
    if(hiocp == NULL)
    {
        m_sharedIocpData->m_listenSock.closeSocket();
        RLOG_ERROR("Create iocp error!");
        return false;
    }
    NetUtil::postAccept(m_sharedIocpData);

    return true;
}

}   //namespace ServerNetwork

#endif
