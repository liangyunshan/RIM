#include "netconnector.h"

#include "../Network/socket.h"
#include "../Network/connection/seriesconnection.h"

#include <climits>
#include <memory>

#ifdef __LOCAL_CONTACT__
void RunNetConnetor(NetFunc func,ParameterSettings::NodeServer * server)
{
    do{
        if(server->localIp.toLocal8Bit().size() > SOCK_CHAR_BUFF_LEN)
            break;

        if(server->localPort.size() == 0 || server->localPort.toUShort() > USHRT_MAX)
            break;

        char buff[SOCK_CHAR_BUFF_LEN] = {0};
        memcpy(buff,server->localIp.toLocal8Bit().data(),server->localIp.toLocal8Bit().size());
        unsigned short port = server->localPort.toUShort();

        ServerNetwork::RSocket socket;
        if(socket.createSocket()){
            if(socket.connect(buff,port)){
                std::shared_ptr<ServerNetwork::SeriesConnection> sc = std::make_shared<ServerNetwork::SeriesConnection>();
                sc->setIp(buff);
                sc->setPort(port);
                sc->setNodeId(server->nodeId);
                sc->setSocket(socket.getSocket());

                ServerNetwork::SeriesConnectionManager::instance()->addConnection(sc);

                func(server->nodeId,true,socket.getSocket());
                return;
            }
        }
    }while(0);

    func(server->nodeId,false,-1);
}

NetConnector::NetConnector(QObject * parent):
    QThread(parent),dataReady(false)
{
    memset(buff,0,SOCK_CHAR_BUFF_LEN);
}

NetConnector::~NetConnector()
{
    wait();
}

void NetConnector::bindServer(ParameterSettings::NodeServer & server)
{
    if(server.localIp.toLocal8Bit().size() > SOCK_CHAR_BUFF_LEN)
        return;

    if(server.localPort.size() == 0 || server.localPort.toUShort() > USHRT_MAX)
        return;

    memcpy(buff,server.localIp.toLocal8Bit().data(),server.localIp.toLocal8Bit().size());
    port = server.localPort.toUShort();
    nodeId = server.nodeId;
    dataReady = true;
}

void NetConnector::registCallBack(NetFunc func)
{
    nfc = func;
}

void NetConnector::run()
{
    if(dataReady){
        ServerNetwork::RSocket socket;
        if(socket.createSocket()){
            if(socket.connect(buff,port)){
                std::shared_ptr<ServerNetwork::SeriesConnection> sc = std::make_shared<ServerNetwork::SeriesConnection>();
                sc->setIp(buff);
                sc->setPort(port);
                sc->setNodeId(nodeId);
                sc->setSocket(socket.getSocket());

                ServerNetwork::SeriesConnectionManager::instance()->addConnection(sc);

//                nfc(this,nodeId,true,socket.getSocket());
                return;
            }
        }
    }

//    nfc(this,nodeId,false,-1);
}
#endif
