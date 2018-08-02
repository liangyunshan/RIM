#include "seriesconnection.h"

#include "Util/rlog.h"

namespace ServerNetwork {

SeriesConnection::SeriesConnection():
    cPort(0),cSocket(0)
{
    memset(cIp,0,SOCK_CHAR_BUFF_LEN);
}

void SeriesConnection::setIp(const char *destIp)
{
    if(destIp == nullptr)
        return;

    if(strlen(destIp) > SOCK_CHAR_BUFF_LEN )
        return;

    memset(cIp,0,SOCK_CHAR_BUFF_LEN);
    strcpy(cIp,destIp);
}

void SeriesConnection::setPort(unsigned short destPort)
{
    this->cPort = destPort;
}

SeriesConnectionManager * SeriesConnectionManager::manager = nullptr;

SeriesConnectionManager::SeriesConnectionManager()
{
    manager = this;
}

SeriesConnectionManager *SeriesConnectionManager::instance()
{
    if(manager ==  NULL)
        manager = new SeriesConnectionManager();

    return manager;
}

void SeriesConnectionManager::addConnection(std::shared_ptr<SeriesConnection> conn)
{
    if(conn == nullptr)
        return;

    std::unique_lock<std::mutex> ul(mutex);
    connList.push_back(conn);
}

void SeriesConnectionManager::remove(std::shared_ptr<SeriesConnection> conn)
{
    std::lock_guard<std::mutex> lg(mutex);
    if(conn && connList.size() > 0)
    {
        RLOG_INFO("Remove client %d,",conn->socket());
        conn.reset();
        connList.remove(conn);
    }
}

void SeriesConnectionManager::remove(unsigned short nodeId)
{
    std::unique_lock<std::mutex> ul(mutex);

    auto index = std::find_if(connList.begin(),connList.end(),[&nodeId](std::shared_ptr<SeriesConnection> conn){
        return conn->getNodeId() == nodeId;
    });

    if(index != connList.end()){
        (*index).reset();
        connList.erase(index);
    }
}

void SeriesConnectionManager::removeFd(int sockdId)
{
    std::unique_lock<std::mutex> ul(mutex);

    auto index = std::find_if(connList.begin(),connList.end(),[&sockdId](std::shared_ptr<SeriesConnection> conn){
        return conn->socket() == sockdId;
    });

    if(index != connList.end()){
        (*index).reset();
        connList.erase(index);
    }
}

void SeriesConnectionManager::removeAll()
{
    std::lock_guard<std::mutex> lg(mutex);
    std::for_each(connList.begin(),connList.end(),[](std::shared_ptr<SeriesConnection> client){
        client.reset();
    });
    connList.clear();
}

std::shared_ptr<SeriesConnection> SeriesConnectionManager::getConnection(int sock)
{
    std::lock_guard<std::mutex> lg(mutex);
    auto findIndex = std::find_if(connList.begin(),connList.end(),[&sock](std::shared_ptr<SeriesConnection> client){
        if(client->socket() == sock)
            return true;
        return false;
    });

    if(findIndex != connList.end()){
        return (*findIndex);
    }

    return nullptr;
}

std::shared_ptr<SeriesConnection> SeriesConnectionManager::getConnection(unsigned short nodeId)
{
    std::lock_guard<std::mutex> lg(mutex);

    auto findIndex = std::find_if(connList.begin(),connList.end(),[&nodeId](std::shared_ptr<SeriesConnection> client){
        if(client->nodeId == nodeId)
            return true;
        return false;
    });

    if(findIndex != connList.end()){
        return (*findIndex);
    }

    return nullptr;
}

}
