#ifndef NETWORK_GLOBAL_H
#define NETWORK_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QByteArray>
#include <QLinkedList>

#if defined(NETWORK_LIBRARY)
#  define NETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

#define SEND_MAGIC_NUM 0x7DBCD6AC               //发送魔数
#define RECV_MAGIC_NUM 0xD7CB6DCA               //接收魔数

#define MAX_PACKET 1024                         //发送数据时一次最大数据长度(不包括前后的控制信息)
#define MAX_SEND_BUFF (MAX_PACKET + 24)

typedef int(*Func)(const char *,const int);

struct DataPacket
{
    unsigned int magicNum;                   //魔数
    unsigned int packId;                     //数据包身份，一个id可对应多个帧，也可以有一个帧
    unsigned int currentIndex;               //当前帧
    unsigned int totalIndex;                 //总帧数量
    unsigned int currentLen;                 //当前数据长度
    unsigned int totalLen;                   //总数据长度
};

//数据包接收缓冲
struct PacketBuff
{
    PacketBuff()
    {
        totalPackIndex = 0;
        recvPackIndex = 0;
        recvSize = 0;
        isCompleted = false;
    }

    QByteArray getFullData()
    {
        QByteArray data;
        if(isCompleted && recvSize > 0)
        {
            while(!buff.isEmpty())
            {
                data.append(buff.takeFirst());
            }
        }
        return data;
    }

    bool isCompleted;                           //该缓冲数据是否完整
    unsigned int recvSize;                      //接收数据的长度
    unsigned short totalPackIndex;              //总数据包分包大小
    unsigned short recvPackIndex;               //已经接收到数据的索引，在recvPackIndex==totalPackIndex时由处理线程进行重新组包
    QLinkedList<QByteArray> buff;               //存放接收到数据(不包含网络数据头DataPacket)，插入时recvPackIndex+1
};

/*!
 *  @brief 传输方式
 *  @details 数据从本机出去的方式包含TCP、UDP、总线，至于北斗等方式，也是UDP传输至中间某一台位后，在转发。
 */
enum CommMethod{
    C_UDP,      /*!< UDP方式 */
    C_TCP,      /*!< TCP方式 */
    C_BUS       /*!< 总线方式 */
};

/*!
 *  @brief 待发送数据单元描述，包含了发送时使用的方式，以及待发送的数据
 */
struct SendUnit
{
    CommMethod method;              /*!< 发送方式 */
    QByteArray data;                /*!< 待发送的数据 */
};

#endif // NETWORK_GLOBAL_H
