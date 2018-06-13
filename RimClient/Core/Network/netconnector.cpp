#include "netconnector.h"

#include <QDebug>

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <windows.h>
typedef  int socklen_t;
#elif defined(Q_OS_UNIX)
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define closesocket close
#endif

#include "Network/win32net/msgsender.h"
#include "Network/win32net/msgreceive.h"
#include "Network/multitransmits/tcptransmit.h"
#include "Network/multitransmits/ddstransmit.h"
#include "Util/rlog.h"
#include "global.h"
#include "rsingleton.h"
#include "messdiapatch.h"

SuperConnector::SuperConnector(QObject *parent):ClientNetwork::RTask(parent),
    netConnected(false),delayTime(3),command(Net_None),tcpTransmit(nullptr)
{

}

SuperConnector::~SuperConnector()
{
    if(netConnected && rsocket->isValid()){
        if(rsocket->closeSocket())
            netConnected = false;
    }
}

/*!
 * @brief 网络连接
 * @param[in] delayTime 默认延迟时间为3s
 */
void SuperConnector::connect(int delaySecTime)
{
    command = Net_Connect;
    delaySecTime = delaySecTime;
    condition.wakeOne();
}

void SuperConnector::reconnect(int delaySecTime)
{
    command = Net_Reconnect;
    delayTime = delaySecTime;
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


TextNetConnector * TextNetConnector::netConnector = nullptr;

TextNetConnector::TextNetConnector():
    SuperConnector()
{
    netConnector = this;

    tcpTransmit = new ClientNetwork::TcpTransmit();

    msgSender = new ClientNetwork::TextSender();
    QObject::connect(msgSender,SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

    msgReceive = new ClientNetwork::TextReceive();
    QObject::connect(msgReceive,SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

//    msgSender->addTransmit(tcpTransmit->type(),tcpTransmit);
//    msgReceive->bindTransmit(tcpTransmit);

    //716
    ddsTransmit =new ClientNetwork::DDSTransmit();
    msgSender->addTransmit(ddsTransmit->type(),ddsTransmit);
    msgReceive->bindTransmit(ddsTransmit);
}

TextNetConnector::~TextNetConnector()
{
    netConnector = nullptr;
    delete msgReceive;
    delete msgSender;
    stopMe();
    wait();
}

TextNetConnector *TextNetConnector::instance()
{
    return netConnector;
}

void TextNetConnector::respSocketError(CommMethod method)
{
    switch(method){
        case C_TCP:

            break;
        default:
            break;
    }

    MessDiapatch::instance()->onTextSocketError();

    netConnected = false;
//    msgSender->stopMe();
//    msgReceive->stopMe();
}

void TextNetConnector::doConnect()
{
    if(tcpTransmit && !tcpTransmit->connected()){
        char ip[50] = {0};
        memcpy(ip,G_TextServerIp.toLocal8Bit().data(),G_TextServerIp.toLocal8Bit().size());
        bool result = tcpTransmit->connect(ip,G_TextServerPort,delayTime);
        MessDiapatch::instance()->onTextConnected(result);
    }

    if(!msgSender->isRunning())
        msgSender->startMe();

    if(!msgReceive->isRunning())
        msgReceive->startMe();
}

void TextNetConnector::doReconnect()
{
    doConnect();
}

void TextNetConnector::doDisconnect()
{
    if(tcpTransmit)
        tcpTransmit->close();

    msgSender->stopMe();
    msgReceive->stopMe();
}

FileNetConnector * FileNetConnector::netConnector = nullptr;

FileNetConnector::FileNetConnector():
    SuperConnector()
{
    netConnector = this;

    tcpTransmit = new ClientNetwork::TcpTransmit();

    msgSender = new ClientNetwork::FileSender();
    QObject::connect(msgSender,SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

    msgReceive = new ClientNetwork::FileReceive();
    QObject::connect(msgReceive,SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

    msgSender->addTransmit(tcpTransmit->type(),tcpTransmit);
    msgReceive->bindTransmit(tcpTransmit);
}

FileNetConnector::~FileNetConnector()
{
    netConnector = nullptr;
    delete msgSender;
    delete msgReceive;

    stopMe();
    wait();
}

FileNetConnector *FileNetConnector::instance()
{
    return netConnector;
}

void FileNetConnector::respSocketError(CommMethod method)
{
    switch(method){
        case C_TCP:

            break;
        default:
            break;
    }

    MessDiapatch::instance()->onFileSocketError();

    netConnected = false;
//    msgSender->stopMe();
//    msgReceive->stopMe();
}

void FileNetConnector::doConnect()
{
    if(tcpTransmit && !tcpTransmit->connected()){
        char ip[50] = {0};
        memcpy(ip,G_FileServerIp.toLocal8Bit().data(),G_FileServerIp.toLocal8Bit().size());
        bool result = tcpTransmit->connect(ip,G_FileServerPort,delayTime);
        MessDiapatch::instance()->onFileConnected(result);
    }

    if(!msgSender->isRunning())
        msgSender->startMe();

    if(!msgReceive->isRunning())
        msgReceive->startMe();
}

void FileNetConnector::doReconnect()
{
    doConnect();
}

void FileNetConnector::doDisconnect()
{
    if(tcpTransmit)
        tcpTransmit->close();

//    MessDiapatch::instance()->onFileSocketError();

    msgSender->stopMe();
    msgReceive->stopMe();
}
