/*!
 *  @brief     客户端/客户端管理
 *  @details   封装了客户端的基础描述信息/记录所有连接的客户端信息，支持添加、删除
 *  @file      tcpclient.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.09
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QList>
#include <QString>
#include <QMutex>
#include <QLinkedList>
#include <QHash>
#include "network_global.h"

namespace ServerNetwork {

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

    ~PacketBuff()
    {
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

class NETWORKSHARED_EXPORT TcpClient
{
public:
    static TcpClient * create();
    int socket() const {return cSocket;}
    QString ip() const {return QString(cIp);}
    unsigned short port() const {return cPort;}

    QHash<int,PacketBuff*> & getPacketBuffs(){return packetBuffs;}
    QByteArray & getHalfPacketBuff(){return halfPackBufff;}

    void lock(){mutex.lock();}
    void unLock(){mutex.unlock();}

private:
    explicit TcpClient();
    ~TcpClient();
    char cIp[32];
    unsigned short cPort;
    int cSocket;

    /*!
       @details 因为IOCP接收数据后，将数据从缓冲区拷贝至对应的缓冲区之后，才进行下一次的接收投递请求。
                这样即时断包，断的数据也是从头部截断，下一次读取数据后，发现头部不完整，则只需要从断包缓冲区将信息拿出拼接至此次数据头部即可。
    */
    QByteArray halfPackBufff;                       //非完整包缓冲区
    QHash<int,PacketBuff*> packetBuffs;             //多包缓冲区
    QMutex mutex;

    friend class TcpClientManager;
};

class NETWORKSHARED_EXPORT TcpClientManager
{
public:
    TcpClientManager();
    static TcpClientManager * instance();

    void addClient(TcpClient * client);

    void remove(TcpClient * client);
    void removeAll();

    TcpClient *  getClient(int sock);
    TcpClient *  addClient(int sockId, char* ip, unsigned short port);

    int counts();

private:
    static TcpClientManager * manager;

    QList<TcpClient *> clientList;

    QMutex mutex;
};

}   //namespace ServerNetwork

#endif // TCPCLIENT_H
