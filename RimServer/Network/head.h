#ifndef HEAD_H
#define HEAD_H

#include <QByteArray>

#include <WinSock2.h>
#include <Windows.h>

#define IO_BUFF_SIZE  1024
#define MAX_RECV_SIZE 1024*8                    //接收缓冲区长度

#define MAX_PACKET 1024                         //发送数据时一次最大数据长度(不包括前后的控制信息)
#define MAX_SEND_BUFF (MAX_PACKET + 24)

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

#define RECV_MAGIC_NUM 0x7DBCD6AC            //与客户端相对应
#define SEND_MAGIC_NUM 0xD7CB6DCA

struct DataPacket
{
    unsigned int magicNum;                   //魔数，0x7DBCD6AC
    unsigned int packId;                     //数据包身份，一个id可对应多个帧，也可以有一个帧
    unsigned int currentIndex;               //当前帧
    unsigned int totalIndex;                 //总帧数量
    unsigned int currentLen;                 //当前数据长度
    unsigned int totalLen;                   //总数据长度
};


#endif // HEAD_H
