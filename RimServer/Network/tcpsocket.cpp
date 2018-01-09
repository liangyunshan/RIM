#include "tcpsocket.h"

#include <QtGlobal>

#include "Util/rlog.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#pragma  comment(lib,"ws2_32.lib")
#elif defined(Q_OS_UNIX)

#endif

TcpSocket::TcpSocket()
{
    tcpSocket = 0;
    socketPort = 0;
    socketValid = false;
    memset(socketIp,0,sizeof(socketIp));
}

bool TcpSocket::createSocket()
{
#ifdef Q_OS_WIN
    static bool isInit = false;
    if(!isInit)
    {
        isInit = true;
        WSADATA ws;
        if(WSAStartup(MAKEWORD(2,2),&ws) != 0)
        {
            RLOG_ERROR("Init windows socket failed!");
            return false;
        }
    }
#endif

    tcpSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(tcpSocket == INVALID_SOCKET)
    {
        RLOG_ERROR("Create socket failed!");
        return false;
    }

    RLOG_INFO("Create socket success!");

    socketValid = true;

    return true;
}

bool TcpSocket::bind(const char *ip, unsigned short port)
{
    if(!isValid())
    {
        return false;
    }

    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);

    int ret = ::bind(tcpSocket,(sockaddr*)&saddr,sizeof(saddr));
    if(ret != 0)
    {
        RLOG_ERROR("Bind socket error [%s:%d]",ip,port);
        return false;
    }

    RLOG_INFO("Bind socket success!");

    strcpy(socketIp,ip);
    socketPort = port;

    return true;
}

bool TcpSocket::listen()
{
    if(!isValid())
    {
        return false;
    }

    int ret = ::listen(tcpSocket,10);
    if(ret != 0)
    {
        RLOG_ERROR("Listen socket error!");
        return false;
    }

    RLOG_INFO("Listen socket success!");

    return true;
}

void TcpSocket::closeSocket()
{
    if(isValid())
    {
        closesocket(tcpSocket);
        tcpSocket = 0;
    }
}

TcpSocket TcpSocket::accept()
{
    TcpSocket tmpSocket;
    if(!isValid())
    {
        return tmpSocket;
    }
    sockaddr_in clientAddr;
    int len = sizeof(clientAddr);

    while(true)
    {
        SOCKET clientSocket = ::accept(tcpSocket,(sockaddr*)&clientAddr,&len);
        if(clientSocket == INVALID_SOCKET)
        {
            RLOG_ERROR("Accept failed %s",GetLastError());
            break;
        }

        strcpy(tmpSocket.socketIp,inet_ntoa(clientAddr.sin_addr));
        tmpSocket.socketValid = true;
        tmpSocket.socketPort= ntohs(clientAddr.sin_port);
        tmpSocket.tcpSocket = clientSocket;

        RLOG_INFO("Recv socket [%s:%d]",tmpSocket.socketIp,tmpSocket.socketPort);
    }
}

int TcpSocket::recv(char *buff, int length)
{
    if(!isValid() || buff == NULL)
    {
        return -1;
    }

    int buffLen = strlen(buff);
    memset(buff,0,buffLen);

    int ret = ::recv(tcpSocket,buff,length,0);

    return ret;
}

int TcpSocket::send(const char *buff, const int length)
{
    if(!isValid() || buff == NULL)
    {
        return -1;
    }

    int sendLen = 0;
    while(sendLen != length)
    {
        int ret = ::send(tcpSocket,buff+sendLen,length-sendLen,0);
        if (ret <= 0)
        {
            break;
        }
        sendLen += ret;
    }
    return sendLen;
}
