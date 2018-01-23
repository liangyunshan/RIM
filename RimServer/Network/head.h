#ifndef HEAD_H
#define HEAD_H

#include <QByteArray>

#include <WinSock2.h>
#include <Windows.h>

#define IO_BUFF_SIZE  1024
#define MAX_RECV_SIZE 1024*8                    //接收缓冲区长度

/*!
 *  @brief 数据操作类型
 */
enum SocketOperateType
{
    SOCKET_TEXT,
    SOCKET_FILE,
    SOCKET_IMG
};

/*!
 *  @brief 内部通用数据处理结构体
 *  @details 通过对操作类型的分类，归纳出不同操作类型锁需要的数据信息。处理线程根据对应的信息去执行相应的操作；
 */
struct SocketInData
{
    SocketOperateType type;
    SOCKET sockId;
    QByteArray data;
};

struct SocketOutData
{
    SOCKET sockId;
    QByteArray data;
};


#endif // HEAD_H
