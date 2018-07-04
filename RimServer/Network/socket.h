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

#ifndef RSOCKET_H
#define RSOCKET_H

#include "network_global.h"

namespace ServerNetwork {

class NETWORKSHARED_EXPORT  RSocket
{
public:
    RSocket();

    bool createSocket();
    bool closeSocket();
    bool bind(const char * ip,unsigned short port);
    bool listen();
    RSocket accept();

    unsigned short port(){return socketPort;}

    int recv(char * buff,int length);
    int send(const char * buff,const int length);

    bool connect(const char * remoteIp,const unsigned short remotePort,int timeouts = 3);

    bool setBlock(bool flag);
    bool isBock(){return blockAble;}

    int setSockopt(int optname,const char * optval,int optlen);

    bool isValid(){return socketValid;}

    int getLastError();

    int getSocket()const {return tcpSocket;}

private:
    int getErrorCode();

private:
    char socketIp[20];
    unsigned short socketPort;

    bool socketValid;
    bool blockAble;

    int tcpSocket;

    int errorCode;
};

}   //namespace ServerNetwork

#endif // TCPSOCKET_H
