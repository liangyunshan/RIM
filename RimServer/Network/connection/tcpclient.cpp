#include "tcpclient.h"

#include <QMutexLocker>

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

void TcpClient::setIp(const char *destIp)
{
    if(destIp == nullptr)
        return;

    if(strlen(destIp) > SOCK_CHAR_BUFF_LEN )
        return;

    memset(cIp,0,SOCK_CHAR_BUFF_LEN);
    strcpy(cIp,destIp);
}

void TcpClient::setPort(unsigned short cPort)
{
    this->cPort = cPort;
}

int TcpClient::getPackId()
{
    std::lock_guard<std::mutex> lg(packIdMutex);
    sendPackId++;

    return sendPackId;
}

bool TcpClient::addFile(QString fileId, FileRecvDesc *desc)
{
    if(desc == NULL)
        return false;

    std::lock_guard<std::mutex> lg(fileMutex);
    fileRecvList.insert(fileId,desc);
    return true;
}

bool TcpClient::removeFile(QString &fileId)
{
    std::lock_guard<std::mutex> lg(fileMutex);

    if(fileRecvList.contains(fileId))
    {
        FileRecvDesc * desc = fileRecvList.value(fileId);
        fileRecvList.remove(fileId);
        delete desc;
        return true;
    }
    return false;
}

FileRecvDesc *TcpClient::getFile(QString fileId)
{
    std::lock_guard<std::mutex> lg(fileMutex);
    if(fileRecvList.contains(fileId)){
        return fileRecvList.value(fileId);
    }
    return NULL;
}

#ifdef __LOCAL_CONTACT__

bool TcpClient::addFileId(RecvFileTypeId fileId)
{
    std::lock_guard<std::recursive_mutex> lg(fileTypeMutex);

    QDB2051::FileType pType;
    if(queryFiletype(fileId,pType))
        return true;

    fileTypeVecs.push_back(fileId);

    return true;
}

void TcpClient::removeFileId(RecvFileTypeId fileId)
{
    std::lock_guard<std::recursive_mutex> lg(fileTypeMutex);

    auto beg = fileTypeVecs.begin();
    while(beg != fileTypeVecs.end()){
        if(*beg == fileId){
           beg = fileTypeVecs.erase(beg);
           continue;
        }
        beg++;
    }
}

/*!
 * @brief 根据指定的FileTypeId查询信息类型
 * @param[in] fileId 待查询Id
 * @param[in/out] pType 对应文件类型
 * @return 存在返回true，不存在返回false
 */
bool TcpClient::queryFiletype(RecvFileTypeId &fileId, QDB2051::FileType &pType)
{
    std::lock_guard<std::recursive_mutex> lg(fileTypeMutex);
    auto findex = std::find_if(fileTypeVecs.begin(),fileTypeVecs.end(),[&](RecvFileTypeId & existFileId){
        return existFileId == fileId;
    });

    if(findex == fileTypeVecs.end())
        return false;

    pType = (*findex).filetype;

    return true;
}

#endif

TcpClient::TcpClient()
{
    memset(cIp,0,32);

    sendPackId = qrand()%1024 + 1000;
    onlineState = 0;
    cSocket = 0;
    cPort = 0;
}

TcpClient::~TcpClient()
{
    QHashIterator<int,PacketBuff*> iter(packetBuffs);
    while(iter.hasNext())
    {
         iter.next();
         delete iter.value();
    }
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
    std::lock_guard<std::mutex> lg(mutex);
    clientList.push_back(client);
}

void TcpClientManager::remove(TcpClient *client)
{
    std::lock_guard<std::mutex> lg(mutex);
    if(client && clientList.size() > 0)
    {
        RLOG_INFO("Remove client %d,",client->socket());
        delete client;
        clientList.remove(client);
    }
}

void TcpClientManager::removeAll()
{
    std::lock_guard<std::mutex> lg(mutex);
    std::for_each(clientList.begin(),clientList.end(),[](TcpClient * client){
        delete client;
    });

    clientList.clear();
}

TcpClient *TcpClientManager::getClient(int sock)
{
    std::lock_guard<std::mutex> lg(mutex);
    auto findIndex = std::find_if(clientList.begin(),clientList.end(),[&sock](TcpClient * client){
        if(client->socket() == sock)
            return true;
        return false;
    });

    if(findIndex != clientList.end()){
        return (*findIndex);
    }

    return NULL;
}

TcpClient *TcpClientManager::getClient(QString accountId)
{
    std::lock_guard<std::mutex> lg(mutex);

    auto findIndex = std::find_if(clientList.begin(),clientList.end(),[&accountId](TcpClient * client){
        if(client->getAccount() == accountId)
            return true;
        return false;
    });

    if(findIndex != clientList.end()){
        return (*findIndex);
    }

    return NULL;
}

ClientList TcpClientManager::getClients(QString accountId)
{
    std::lock_guard<std::mutex> lg(mutex);
    std::list<TcpClient *> clients;
    std::for_each(clientList.begin(),clientList.end(),[&](TcpClient * client){
        if(client->getAccount() == accountId)
            clients.push_back(client);
    });

    return clients;
}

ClientList TcpClientManager::getClients()
{
    std::lock_guard<std::mutex> lg(mutex);
    return clientList;
}

TcpClient *TcpClientManager::addClient(int sockId, char *ip, unsigned short port)
{
    std::lock_guard<std::mutex> lg(mutex);
    TcpClient * client = new TcpClient;
    client->cSocket = sockId;
    client->cPort = port;
    memcpy(client->cIp,ip,strlen(ip));

    clientList.push_back(client);

    return client;
}

int TcpClientManager::counts()
{
    std::lock_guard<std::mutex> lg(mutex);
    return clientList.size();
}

}   //namespace ServerNetwork
