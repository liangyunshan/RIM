#include "netconnector.h"

#include <QDebug>

#include "Network/win32net/msgsender.h"
#include "Network/win32net/msgreceive.h"
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

    msgSender = new ClientNetwork::TextSender();
    QObject::connect(msgSender,SIGNAL(socketError(int)),this,SLOT(respSocketError(int)));

    msgReceive = new ClientNetwork::TextReceive();
    QObject::connect(msgReceive,SIGNAL(socketError(int)),this,SLOT(respSocketError(int)));
}

TextNetConnector::~TextNetConnector()
{
    delete msgReceive;
    delete msgSender;

    stopMe();
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

    emit socketError();
}

void TextNetConnector::doConnect()
{
    netConnected = false;
    if(!rsocket->isValid() && rsocket->createSocket())
    {
        msgSender->setSock(rsocket);
        msgReceive->setSock(rsocket);

        char ip[20] = {0};
        memcpy(ip,G_TextServerIp.toLocal8Bit().data(),G_TextServerIp.toLocal8Bit().size());

        if(!netConnected && rsocket->isValid() && rsocket->connect(ip,G_TextServerPort,delayTime))
        {
            netConnected = true;

            msgSender->startMe();
            msgReceive->startMe();
        }else{
            rsocket->closeSocket();
        }
    }else{
        rsocket->closeSocket();
    }

    emit connected(netConnected);
}

void TextNetConnector::doReconnect()
{
    netConnected = false;

    if(!rsocket->isValid() && rsocket->createSocket())
    {
        char ip[20] = {0};
        memcpy(ip,G_TextServerIp.toLocal8Bit().data(),G_TextServerIp.toLocal8Bit().size());

        if(rsocket->isValid() && rsocket->connect(ip,G_TextServerPort,delayTime)){
            netConnected = true;

            msgSender->startMe();
            msgReceive->startMe();
        }else{
            rsocket->closeSocket();
            RLOG_ERROR("Reconnect [%s:%d] error",ip,G_TextServerPort);
        }
    }else{
        rsocket->closeSocket();
    }

    emit connected(netConnected);
}

void TextNetConnector::doDisconnect()
{
    if(netConnected && rsocket->isValid())
    {
        if(rsocket->closeSocket())
        {
            netConnected = false;
            msgSender->stopMe();
            msgReceive->stopMe();
        }
    }
}

FileNetConnector * FileNetConnector::netConnector = NULL;

FileNetConnector::FileNetConnector():
    SuperConnector()
{
    netConnector = this;

    rsocket = new ClientNetwork::RSocket();

    msgSender = new ClientNetwork::FileSender();
    QObject::connect(msgSender,SIGNAL(socketError(int)),this,SLOT(respSocketError(int)));

    msgReceive = new ClientNetwork::FileReceive();
    QObject::connect(msgReceive,SIGNAL(socketError(int)),this,SLOT(respSocketError(int)));
}

FileNetConnector::~FileNetConnector()
{
    delete msgSender;
    delete msgReceive;

    stopMe();
    wait();
}

FileNetConnector *FileNetConnector::instance()
{
    return netConnector;
}

void FileNetConnector::respSocketError(int errorCode)
{
    RLOG_ERROR("Socket close! ErrorCode[%d]",errorCode);

    netConnected = false;
    msgSender->stopMe();
    msgReceive->stopMe();
    emit socketError();
}

void FileNetConnector::doConnect()
{
    netConnected = false;
    if(!rsocket->isValid() && rsocket->createSocket())
    {
        msgSender->setSock(rsocket);
        msgReceive->setSock(rsocket);
        char ip[20] = {0};
        memcpy(ip,G_FileServerIp.toLocal8Bit().data(),G_FileServerIp.toLocal8Bit().size());

        if(!netConnected && rsocket->isValid() && rsocket->connect(ip,G_FileServerPort,delayTime))
        {
            netConnected = true;
            msgSender->startMe();
            msgReceive->startMe();
        }else{
            rsocket->closeSocket();
        }
    }else{
        rsocket->closeSocket();
    }

    emit connected(netConnected);
}

void FileNetConnector::doReconnect()
{
    netConnected = false;

    if(!rsocket->isValid() && rsocket->createSocket())
    {
        char ip[20] = {0};
        memcpy(ip,G_FileServerIp.toLocal8Bit().data(),G_FileServerIp.toLocal8Bit().size());
        if(rsocket->isValid() && rsocket->connect(ip,G_FileServerPort,delayTime))
        {
            netConnected = true;
            msgSender->startMe();
            msgReceive->startMe();
        }else{
            rsocket->closeSocket();
            RLOG_ERROR("Reconnect [%s:%d] error",ip,G_FileServerPort);
        }
    }else{
        rsocket->closeSocket();
    }

    emit connected(netConnected);
}

void FileNetConnector::doDisconnect()
{
    if(netConnected && rsocket->isValid())
    {
        if(rsocket->closeSocket())
        {
            netConnected = false;
            emit connected(netConnected);

            msgSender->stopMe();
            msgReceive->stopMe();
        }
    }
}
