#include "tcpclient.h"

#include <QMutexLocker>
#include <QDebug>

#include "Util/rlog.h"

namespace ServerNetwork {

TcpClient *TcpClient::create()
{
    TcpClient * client = new TcpClient();

    client->cSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(client->cSocket == INVALID_SOCKET)
    {
        RLOG_ERROR("Create socket error,ErrorCode:%d",WSAGetLastError());
        delete client;
        return NULL;
    }

    return client;
}

int TcpClient::getPackId()
{
    QMutexLocker locker(&packIdMutex);
    sendPackId++;

    return sendPackId;
}

TcpClient::TcpClient()
{
    memset(cIp,0,32);

    sendPackId = qrand()%1024 + 1000;

    cSocket = 0;
    cPort = 0;
}

TcpClient::~TcpClient()
{
    QHashIterator<int,PacketBuff*> iter(packetBuffs);
    while (iter.hasNext())
    {
         iter.next();
         delete iter.value();
    }
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"Delete TcpClient";
}

TcpClientManager * TcpClientManager::manager = NULL;

TcpClientManager::TcpClientManager()
{
    manager = this;
}

TcpClientManager *TcpClientManager::instance()
{
    if(manager ==  NULL)
    {
        manager = new TcpClientManager();
    }
    return manager;
}

void TcpClientManager::addClient(TcpClient *client)
{
    QMutexLocker locker(&mutex);
    clientList.append(client);
}

void TcpClientManager::remove(TcpClient *client)
{
    QMutexLocker locker(&mutex);
    if(client && clientList.size() > 0)
    {
        RLOG_INFO("Remove client %d,",client->socket());
        delete client;
        clientList.removeOne(client);
    }
}

void TcpClientManager::removeAll()
{
    for(int i = 0; i < clientList.size(); i++)
    {
        delete clientList.at(i);
    }
    clientList.clear();
}

TcpClient *TcpClientManager::getClient(int sock)
{
    QMutexLocker locker(&mutex);
    QList<TcpClient *>::iterator iter = clientList.begin();
    while(iter != clientList.end())
    {
        if((*iter)->socket() == sock)
        {
            return (*iter);
        }
        iter++;
    }
    return NULL;
}

TcpClient *TcpClientManager::getClient(QString accountId)
{
    QMutexLocker locker(&mutex);
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<clientList.size();
    QList<TcpClient *>::iterator iter = clientList.begin();
    while(iter != clientList.end())
    {
        qDebug()<<(*iter)->getAccount();
        if((*iter)->getAccount() == accountId)
        {
            return (*iter);
        }
        iter++;
    }
    return NULL;
}

TcpClient *TcpClientManager::addClient(int sockId, char *ip, unsigned short port)
{
    QMutexLocker locker(&mutex);
    TcpClient * client = new TcpClient;
    client->cSocket = sockId;
    client->cPort = port;
    memcpy(client->cIp,ip,strlen(ip));

    clientList.append(client);

    return client;
}

int TcpClientManager::counts()
{
    QMutexLocker locker(&mutex);
    return clientList.size();
}

}   //namespace ServerNetwork
