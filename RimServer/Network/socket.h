/*!
 *  @brief     windows底层socket
 *  @details   封装了操作系统底层的socket，支持windows和linux平台的运行
 *  @file      socket.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.09
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "network_global.h"

#ifdef Q_OS_WIN
#include <WinSock2.h>
typedef  int socklen_t;
#elif defined(Q_OS_UNIX)
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define closesocket close
#endif


namespace ServerNetwork {

class NETWORKSHARED_EXPORT  Socket
{
public:
    Socket();

    bool createSocket();
    bool closeSocket();
    bool bind(const char * ip,unsigned short port);
    bool listen();
    Socket accept();

    unsigned short port(){return socketPort;}

    int recv(char * buff,int length);
    int send(const char * buff,const int length);

    bool connect(const char * remoteIp,const unsigned short remotePort,int timeouts = 1000);

    bool setBlock(bool flag);
    bool isBock(){return blockAble;}

    bool isValid(){return socketValid;}

    int getSocket()const {return tcpSocket;}

private:
    char socketIp[20];
    unsigned short socketPort;

    bool socketValid;
    bool blockAble;

    SOCKET tcpSocket;
};

}   //namespace ServerNetwork

#endif // TCPSOCKET_H
