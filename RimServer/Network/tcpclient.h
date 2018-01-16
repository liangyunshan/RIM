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
#include "network_global.h"

namespace ServerNetwork {

class NETWORKSHARED_EXPORT TcpClient
{
public:
    static TcpClient * create();
    int socket() const {return cSocket;}
    QString ip() const {return QString(cIp);}
    unsigned short port() const {return cPort;}

private:
    explicit TcpClient();
    ~TcpClient();
    char cIp[32];
    unsigned short cPort;
    int cSocket;

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
