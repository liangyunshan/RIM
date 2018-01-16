#include "netconnector.h"

#include "Network/msgsender.h"
#include "Network/msgreceive.h"
#include "Util/rlog.h"


NetConnector::NetConnector()
{
    socket.createSocket();
}

bool NetConnector::connect(int delayMillSecond)
{
    if(socket.isValid() && socket.connect(remoteIp.toLocal8Bit().data(),remotePort,delayMillSecond))
    {
        MsgSender * sender = new MsgSender(socket);
        sender->startMe();

        MsgReceive * receive = new MsgReceive(socket);
        receive->startMe();

        return true;
    }

    return false;
}

bool NetConnector::reconnect(int delayMillSecond)
{
    if(socket.isValid() && socket.connect(remoteIp.toLocal8Bit().data(),remotePort,delayMillSecond))
    {
        return true;
    }
    RLOG_ERROR("Reconnect [%s:%d] error",remoteIp.toLocal8Bit().data(),remotePort);
    return false;
}

void NetConnector::setConnectInfo(const QString ip, const unsigned short port)
{
    remoteIp = ip;
    remotePort = port;
}

bool NetConnector::setBlock(bool flag)
{
    if(socket.isValid())
    {
        return socket.setBlock(flag);
    }
    return false;
}
