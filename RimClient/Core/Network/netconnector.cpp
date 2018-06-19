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
    delayTime(3),command(Net_None)
{

}

SuperConnector::~SuperConnector()
{

}

/*!
 * @brief 网络连接
 * @param[in] delayTime 默认延迟时间为3s
 */
void SuperConnector::connect(int delaySecTime)
{
    command = Net_Connect;
    delaySecTime = delaySecTime;
    condition.notify_one();
}

void SuperConnector::reconnect(int delaySecTime)
{
    command = Net_Reconnect;
    delayTime = delaySecTime;
    condition.notify_one();
}

void SuperConnector::disConnect()
{
    command = Net_Disconnect;
    condition.notify_one();
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
    condition.notify_one();
}

void SuperConnector::run()
{
    runningFlag = true;
    while(runningFlag)
    {
        std::unique_lock<std::mutex> ul(mutex);
        condition.wait(ul);

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

    tcpTransmit = std::make_shared<ClientNetwork::TcpTransmit>();

    msgSender = new ClientNetwork::TextSender();
    QObject::connect(msgSender,SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

    msgReceive = new ClientNetwork::TextReceive();
    QObject::connect(msgReceive,SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

    //716
    ddsTransmit = std::make_shared<ClientNetwork::DDSTransmit>();
    msgSender->addTransmit(ddsTransmit);
    msgReceive->bindTransmit(ddsTransmit);
}

TextNetConnector::~TextNetConnector()
{
    netConnector = nullptr;

    delete msgSender;
    msgSender = nullptr;

    delete msgReceive;
    msgReceive = nullptr;
    stopMe();
    wait();
}

TextNetConnector *TextNetConnector::instance()
{
    return netConnector;
}

/*!
 * @brief 处理某种传输方式中产生异常
 * @param[in] method 传输方式
 * @note  1.当某种传输方式产生了异常，应进行界面提示; \n
 *        2.此时的发送/接收队列任务应不关闭。
 */
void TextNetConnector::respSocketError(CommMethod method)
{
    switch(method){
        case C_TCP:
            MessDiapatch::instance()->onTextSocketError();
            break;
        default:
            break;
    }
}

void TextNetConnector::doConnect()
{
    if(tcpTransmit.get() != nullptr && !tcpTransmit->connected()){
        char ip[50] = {0};
        memcpy(ip,G_TextServerIp.toLocal8Bit().data(),G_TextServerIp.toLocal8Bit().size());
        bool result = tcpTransmit->connect(ip,G_TextServerPort,delayTime);
        MessDiapatch::instance()->onTextConnected(result);
    }

    if(msgSender != nullptr && !msgSender->running())
        msgSender->startMe();

    if(msgReceive != nullptr && !msgReceive->running())
        msgReceive->startMe();
}

void TextNetConnector::doReconnect()
{
    doConnect();
}

void TextNetConnector::doDisconnect()
{
    if(tcpTransmit.get() != nullptr)
        tcpTransmit->close();

    msgSender->stopMe();
    msgReceive->stopMe();
}

FileNetConnector * FileNetConnector::netConnector = nullptr;

FileNetConnector::FileNetConnector():
    SuperConnector()
{
    netConnector = this;

    tcpTransmit = std::make_shared<ClientNetwork::TcpTransmit>();

    msgSender = new ClientNetwork::FileSender();
    QObject::connect(msgSender,SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

    msgReceive = new ClientNetwork::FileReceive();
    QObject::connect(msgReceive,SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

    msgSender->addTransmit(tcpTransmit);
    msgReceive->bindTransmit(tcpTransmit);
}

FileNetConnector::~FileNetConnector()
{
    netConnector = nullptr;

    delete msgSender;
    msgSender = nullptr;

    delete msgReceive;
    msgReceive = nullptr;

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
            MessDiapatch::instance()->onFileSocketError();
            break;
        default:
            break;
    }

//    msgSender->stopMe();
//    msgReceive->stopMe();
}

void FileNetConnector::doConnect()
{
    if(tcpTransmit.get() != nullptr && !tcpTransmit->connected()){
        char ip[50] = {0};
        memcpy(ip,G_FileServerIp.toLocal8Bit().data(),G_FileServerIp.toLocal8Bit().size());
        bool result = tcpTransmit->connect(ip,G_FileServerPort,delayTime);
        MessDiapatch::instance()->onFileConnected(result);
    }

    if(msgSender != nullptr &&!msgSender->running())
        msgSender->startMe();

    if(msgReceive != nullptr && !msgReceive->running())
        msgReceive->startMe();
}

void FileNetConnector::doReconnect()
{
    doConnect();
}

void FileNetConnector::doDisconnect()
{
    if(tcpTransmit.get() != nullptr)
        tcpTransmit->close();

//    MessDiapatch::instance()->onFileSocketError();

    msgSender->stopMe();
    msgReceive->stopMe();
}
