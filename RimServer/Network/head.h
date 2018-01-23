#ifndef HEAD_H
#define HEAD_H

#include <QByteArray>

#include <WinSock2.h>
#include <Windows.h>

#define IO_BUFF_SIZE  1024
#define MAX_RECV_SIZE 1024*8                    //接收缓冲区长度

struct SocketInData
{
    SOCKET sockId;
    QByteArray data;
};

struct SocketOutData
{
    SOCKET sockId;
    QByteArray data;
};


#endif // HEAD_H
