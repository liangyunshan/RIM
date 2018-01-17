#include "netconnector.h"

#include "Network/msgsender.h"
#include "Network/msgreceive.h"
#include "Util/rlog.h"
#include "global.h"


NetConnector::NetConnector()
{
    connected = false;
    socket.createSocket();
}

bool NetConnector::connect(int delayMillSecond)
{
    if(connected)
    {
        return true;
    }

    if(socket.isValid() && socket.connect(G_ServerIp.toLocal8Bit().data(),G_ServerPort,delayMillSecond))
    {
        connected = true;
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
    if(socket.isValid() && socket.connect(G_ServerIp.toLocal8Bit().data(),G_ServerPort,delayMillSecond))
    {
        connected = true;
        return true;
    }
    RLOG_ERROR("Reconnect [%s:%d] error",G_ServerIp.toLocal8Bit().data(),G_ServerPort);
    return false;
}

bool NetConnector::setBlock(bool flag)
{
    if(socket.isValid())
    {
        return socket.setBlock(flag);
    }
    return false;
}
