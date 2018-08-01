/*!
 *  @brief     服务器串联连接
 *  @details   服务器作为客户端时，保存与对方服务器建立连接的通信，此连接只用于服务器向另外服务器发送数据。 \n
 *             1.服务器主动连接对方服务器，当建立连接后成功后，将连接加入至容器保存； \n
 *             2.本服务器接收对方服务器发送的数据，目前使用IOCP建立连接; \n
 *             3.使用对方服务器节点信息判断SeriesConnectionManager和IOCP的TcpClientManager中是否存在目的服务器连接信息。
 *  @author    wey
 *  @version   1.0
 *  @date      2018.07.02
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SERIESCONNECTION_H
#define SERIESCONNECTION_H

#include <QString>
#include <list>
#include <mutex>
#include <memory>

#include "../network_global.h"
#include "../head.h"

namespace ServerNetwork {

class SeriesConnectionManager;

class NETWORKSHARED_EXPORT SeriesConnection
{
public:
    SeriesConnection();
    ~SeriesConnection(){

    }

    void setSocket(int sockId){cSocket = sockId;}
    int socket() const {return cSocket;}

    void setIp(const char * destIp);
    QString ip() const {return QString(cIp);}

    void setPort(unsigned short destPort);
    unsigned short port() const {return cPort;}

    void setNodeId(unsigned short id){this->nodeId = id;}
    unsigned short getNodeId(){return this->nodeId;}

private:
    char cIp[SOCK_CHAR_BUFF_LEN];
    unsigned short cPort;
    int cSocket;

    unsigned short nodeId;

    friend class SeriesConnectionManager;
};


class NETWORKSHARED_EXPORT SeriesConnectionManager
{
public:
    static SeriesConnectionManager * instance();

    void addConnection(std::shared_ptr<SeriesConnection> conn);

    void remove(std::shared_ptr<SeriesConnection> conn);
    void remove(unsigned short nodeId);
    void removeFd(int sockdId);
    void removeAll();

    std::shared_ptr<SeriesConnection>  getConnection(int sock);
    std::shared_ptr<SeriesConnection>  getConnection(unsigned short nodeId);

private:
    explicit SeriesConnectionManager();
    static SeriesConnectionManager * manager;

    std::list<std::shared_ptr<SeriesConnection> > connList;
    std::mutex mutex;

};

}

#endif // SERIESCONNECTION_H
