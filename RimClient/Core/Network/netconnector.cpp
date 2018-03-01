#include "netconnector.h"

#include <QDebug>

#include "Network/msgsender.h"
#include "Network/msgreceive.h"
#include "Util/rlog.h"
#include "global.h"

SuperConnector::SuperConnector(QObject *parent):ClientNetwork::RTask(parent),
    netConnected(false),delayTime(3),command(Net_None)
{

}

void SuperConnector::connect(int delayTime)
{
    command = Net_Connect;
    delayTime = delayTime;
    condition.wakeOne();
}

void SuperConnector::reconnect(int delayTime)
{
    command = Net_Reconnect;
    delayTime = delayTime;
    condition.wakeOne();
}

void SuperConnector::disConnect()
{
    command = Net_Disconnect;
    condition.wakeOne();
}

bool SuperConnector::setBlock(bool flag)
{
    if(rsocket->isValid())
    {
        return rsocket->setBlock(flag);
    }
    return false;
}

void SuperConnector::startMe()
{
    RTask::startMe();

    if(!isRunning())
    {
        start();
    }
}

void SuperConnector::stopMe()
{
    runningFlag = false;
    command = Net_None;
    condition.wakeOne();
}

void SuperConnector::run()
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


TextNetConnector * TextNetConnector::netConnector = NULL;

TextNetConnector::TextNetConnector():
    SuperConnector()
{
    netConnector = this;

    rsocket = new ClientNetwork::RSocket();

    msgSender = new ClientNetwork::MsgSender();
    QObject::connect(msgSender,SIGNAL(socketError(int)),this,SLOT(respSocketError(int)));

    msgReceive = new ClientNetwork::MsgReceive();
    QObject::connect(msgReceive,SIGNAL(socketError(int)),this,SLOT(respSocketError(int)));
}

TextNetConnector::~TextNetConnector()
{
    wait();
}

TextNetConnector *TextNetConnector::instance()
{
    return netConnector;
}


void TextNetConnector::respSocketError(int errorCode)
{
    RLOG_ERROR("Socket close! ErrorCode[%d]",errorCode);

    netConnected = false;
    msgSender->stopMe();
    msgReceive->stopMe();
}

void TextNetConnector::doConnect()
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

void TextNetConnector::doReconnect()
{
    netConnected = false;
    if(rsocket->isValid() && rsocket->connect(G_ServerIp.toLocal8Bit().data(),G_ServerPort,delayTime))
    {
        netConnected = true;
    }
    RLOG_ERROR("Reconnect [%s:%d] error",G_ServerIp.toLocal8Bit().data(),G_ServerPort);

    emit connected(netConnected);
}

void TextNetConnector::doDisconnect()
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
