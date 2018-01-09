/*!
 *  @brief     windows底层socket
 *  @details   封装了操作系统底层的socket，支持windows和linux平台的运行
 *  @file      tcpsocket.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.09
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "network_global.h"

class NETWORKSHARED_EXPORT  TcpSocket
{
public:
    TcpSocket();

    bool createSocket();
    bool bind(const char * ip,unsigned short port);
    bool listen();
    void closeSocket();
    TcpSocket accept();
    unsigned short port(){return socketPort;}

    int recv(char * buff,int length);
    int send(const char * buff,const int length);

    bool isValid(){return socketValid;}

private:
    char socketIp[20];
    unsigned short socketPort;

    bool socketValid;

    int tcpSocket;
};

#endif // TCPSOCKET_H
