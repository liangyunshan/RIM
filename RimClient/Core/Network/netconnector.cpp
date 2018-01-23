#include "netconnector.h"

#include <QDebug>

#include "Network/msgsender.h"
#include "Network/msgreceive.h"
#include "Util/rlog.h"
#include "global.h"

NetConnector * NetConnector::netConnector = NULL;

NetConnector::NetConnector(QObject *parent):
    ClientNetwork::RTask(parent),netConnected(false),
    delayTime(3),command(Net_None)
{
    netConnector = this;

    rsocket = new ClientNetwork::RSocket();

    msgSender = new ClientNetwork::MsgSender();
    QObject::connect(msgSender,SIGNAL(socketError(int)),this,SLOT(respSocketError(int)));

    msgReceive = new ClientNetwork::MsgReceive();
    QObject::connect(msgReceive,SIGNAL(socketError(int)),this,SLOT(respSocketError(int)));
}

NetConnector::~NetConnector()
{
    wait();
}

NetConnector *NetConnector::instance()
{
    return netConnector;
}

void NetConnector::connect(int delayTime)
{
    command = Net_Connect;
    delayTime = delayTime;
    condition.wakeOne();
}

void NetConnector::reconnect(int delayTime)
{
    command = Net_Reconnect;
    delayTime = delayTime;
    condition.wakeOne();
}

void NetConnector::disConnect()
{
    command = Net_Disconnect;
    condition.wakeOne();
}

bool NetConnector::setBlock(bool flag)
{
    if(rsocket->isValid())
    {
        return rsocket->setBlock(flag);
    }
    return false;
}

void NetConnector::startMe()
{
    RTask::startMe();

    if(!isRunning())
    {
        start();
    }
}

void NetConnector::stopMe()
{
    runningFlag = false;
    command = Net_None;
    condition.wakeOne();
}

void NetConnector::respSocketError(int errorCode)
{
    RLOG_ERROR("Socket close! ErrorCode[%d]",errorCode);

    netConnected = false;
    msgSender->stopMe();
    msgReceive->stopMe();
}

void NetConnector::run()
{
    runningFlag = true;
    while(runningFlag)
    {
        mutex.lock();
        condition.wait(&mutex);
        mutex.unlock();

        switch(command)
        {
            case Net_Connect:
                                doConnect();
                                break;
            case Net_Reconnect:
                                doReconnect();
                                break;
            case Net_Disconnect:
                                doDisconnect();
                                break;
            case Net_Heart:
                                break;
            default:
                break;
        }
    }
}

void NetConnector::doConnect()
{
    if(!rsocket->isValid())
    {
        if(rsocket->createSocket())
        {
            msgSender->setSock(rsocket);
            msgReceive->setSock(rsocket);
        }
    }

    if(!netConnected && rsocket->isValid() && rsocket->connect(G_ServerIp.toLocal8Bit().data(),G_ServerPort,delayTime))
    {
        netConnected = true;

        msgSender->startMe();
        msgReceive->startMe();
    }

    emit connected(netConnected);
}

void NetConnector::doReconnect()
{
    netConnected = false;
    if(rsocket->isValid() && rsocket->connect(G_ServerIp.toLocal8Bit().data(),G_ServerPort,delayTime))
    {
        netConnected = true;
    }
    RLOG_ERROR("Reconnect [%s:%d] error",G_ServerIp.toLocal8Bit().data(),G_ServerPort);

    emit connected(netConnected);
}

void NetConnector::doDisconnect()
{
    if(netConnected && rsocket->isValid())
    {
        if(rsocket->closeSocket())
        {
            msgSender->stopMe();
            msgReceive->stopMe();
        }
    }
}
