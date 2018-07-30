#include "netconnector.h"

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

#include <QDebug>

#include "Network/win32net/msgsender.h"
#include "Network/win32net/msgreceive.h"
#include "Network/multitransmits/tcptransmit.h"
#include "Network/multitransmits/ddstransmit.h"
#include "../thread/file716sendtask.h"
#include "Util/rlog.h"
#include "global.h"
#include "util/rsingleton.h"
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
}

TextNetConnector::~TextNetConnector()
{
    netConnector = nullptr;

    msgSender.reset();

    std::for_each(msgReceives.begin(),msgReceives.end(),[](std::pair<CommMethod,std::shared_ptr<ClientNetwork::TextReceive>> item){
        item.second.reset();
    });

    msgReceives.clear();

    stopMe();
    wait();
}

TextNetConnector *TextNetConnector::instance()
{
    return netConnector;
}

/*!
 * @brief 初始化所有可用的数据传输链路
 * @note 1.初始化所有的数据传输链路; \n
 *       2.将每种数据链路添加至数据发送线程; \n
 *       3.为每个数据传输链路创建一个接收线程;
 * @return 是否初始化成功
 */
bool TextNetConnector::initialize()
{
    std::shared_ptr<ClientNetwork::TcpTransmit> tcpTransmit = std::make_shared<ClientNetwork::TcpTransmit>();
    transmits.insert(std::pair<CommMethod,std::shared_ptr<ClientNetwork::BaseTransmit>>(tcpTransmit->type(),tcpTransmit));

//    std::shared_ptr<ClientNetwork::DDSTransmit> ddsTransmit = std::make_shared<ClientNetwork::DDSTransmit>();
//    transmits.insert(std::pair<CommMethod,std::shared_ptr<ClientNetwork::BaseTransmit>>(ddsTransmit->type(),ddsTransmit));

    msgSender = std::make_shared<ClientNetwork::TextSender>();
    QObject::connect(msgSender.get(),SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));

    std::for_each(transmits.begin(),transmits.end(),[&](const std::pair<CommMethod,std::shared_ptr<ClientNetwork::BaseTransmit>> item){
        if(msgSender->addTransmit(item.second)){
            std::shared_ptr<ClientNetwork::TextReceive> msgRecv = std::make_shared<ClientNetwork::TextReceive>();
            QObject::connect(msgRecv.get(),SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));
            msgRecv->bindTransmit(item.second);
            msgReceives.insert(std::pair<CommMethod,std::shared_ptr<ClientNetwork::TextReceive>>(item.second->type(),msgRecv));
        }else{
            MessDiapatch::instance()->onTransmitsInitialError(QObject::tr("Initial transmit %1 error!").arg(item.second->name()));
        }
    });

    return true;
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
            {
                MessDiapatch::instance()->onTextSocketError();
                std::shared_ptr<ClientNetwork::TextReceive> tcpTrans = msgReceives.at(C_TCP);
                if(tcpTrans.get()){
                    tcpTrans->stopMe();
                }
            }
            break;
        default:
            break;
    }
}

/*!
 * @brief 执行数据链路连接
 * @note 只有需要建立连接的链路，才在此处进行连接初始化工作。
 */
void TextNetConnector::doConnect()
{
    std::shared_ptr<ClientNetwork::BaseTransmit> tcpTrans = transmits.at(C_TCP);
    if(tcpTrans.get()!= nullptr && !tcpTrans->connected()){
        char ip[50] = {0};
        memcpy(ip,G_NetSettings.connectedTextIpPort.ip.toLocal8Bit().data(),G_NetSettings.connectedTextIpPort.ip.toLocal8Bit().size());
        bool connected = tcpTrans->connect(ip,G_NetSettings.connectedTextIpPort.port,delayTime);
        G_NetSettings.connectedTextIpPort.setConnected(connected);
        MessDiapatch::instance()->onTextConnected(connected);

        if(connected){
            if(msgReceives.at(C_TCP).get() != nullptr && !msgReceives.at(C_TCP)->running())
                msgReceives.at(C_TCP)->startMe();
        }
    }

    if(msgSender.get() != nullptr && !msgSender->running())
        msgSender->startMe();
}

void TextNetConnector::doReconnect()
{
    doConnect();
}

void TextNetConnector::doDisconnect()
{
    std::for_each(transmits.begin(),transmits.end(),[](std::pair<CommMethod,std::shared_ptr<ClientNetwork::BaseTransmit>> item){
        item.second->close();
    });

    msgSender->stopMe();

    std::for_each(msgReceives.begin(),msgReceives.end(),[](std::pair<CommMethod,std::shared_ptr<ClientNetwork::TextReceive>> item){
        item.second->stopMe();
    });
}

FileNetConnector * FileNetConnector::netConnector = nullptr;

FileNetConnector::FileNetConnector():
    SuperConnector()
{
    netConnector = this;
}

FileNetConnector::~FileNetConnector()
{
    netConnector = nullptr;

    msgSender.reset();

    std::for_each(msgReceives.begin(),msgReceives.end(),[](std::pair<CommMethod,std::shared_ptr<ClientNetwork::FileReceive>> item){
        item.second.reset();
    });

    msgReceives.clear();

    stopMe();
    wait();
}

FileNetConnector *FileNetConnector::instance()
{
    return netConnector;
}

bool FileNetConnector::initialize()
{
    std::shared_ptr<ClientNetwork::TcpTransmit> tcpTransmit = std::make_shared<ClientNetwork::TcpTransmit>();
    transmits.insert(std::pair<CommMethod,std::shared_ptr<ClientNetwork::BaseTransmit>>(tcpTransmit->type(),tcpTransmit));

    msgSender = std::make_shared<ClientNetwork::FileSender>();
    QObject::connect(msgSender.get(),SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));
//    auto callBack = std::bind(&FileNetConnector::processDataFileProgress,this,std::placeholders::_1);

    std::for_each(transmits.begin(),transmits.end(),[&](const std::pair<CommMethod,std::shared_ptr<ClientNetwork::BaseTransmit>> item){
        if(File716SendTask::instance()->addTransmit(item.second)){
            msgSender->addTransmit(item.second);
            std::shared_ptr<ClientNetwork::FileReceive> msgRecv = std::make_shared<ClientNetwork::FileReceive>();
            QObject::connect(msgRecv.get(),SIGNAL(socketError(CommMethod)),this,SLOT(respSocketError(CommMethod)));
            msgRecv->bindTransmit(item.second);
            msgReceives.insert(std::pair<CommMethod,std::shared_ptr<ClientNetwork::FileReceive>>(item.second->type(),msgRecv));
        }else{
            MessDiapatch::instance()->onTransmitsInitialError(QObject::tr("Initial transmit %1 error!").arg(item.second->name()));
        }
    });

    return true;
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
    std::shared_ptr<ClientNetwork::BaseTransmit> tcpTrans = transmits.at(C_TCP);
    if(tcpTrans.get()!= nullptr && !tcpTrans->connected()){
        char ip[50] = {0};

        memcpy(ip,G_NetSettings.connectedFileIpPort.ip.toLocal8Bit().data(),G_NetSettings.connectedFileIpPort.ip.toLocal8Bit().size());
        bool connected = tcpTrans->connect(ip,G_NetSettings.connectedFileIpPort.port,delayTime);
        G_NetSettings.connectedFileIpPort.setConnected(connected);
        MessDiapatch::instance()->onFileConnected(connected);
        if(connected){
            if(msgReceives.at(C_TCP).get() != nullptr && !msgReceives.at(C_TCP)->running())
                msgReceives.at(C_TCP)->startMe();
        }
    }

    if(msgSender.get() != nullptr && !msgSender->running())
        msgSender->startMe();
}

void FileNetConnector::doReconnect()
{
    doConnect();
}

void FileNetConnector::doDisconnect()
{
    std::for_each(transmits.begin(),transmits.end(),[](std::pair<CommMethod,std::shared_ptr<ClientNetwork::BaseTransmit>> item){
        item.second->close();
    });

    if(msgSender.get() != nullptr)
        msgSender->stopMe();

    std::for_each(msgReceives.begin(),msgReceives.end(),[](std::pair<CommMethod,std::shared_ptr<ClientNetwork::FileReceive>> item){
        item.second->stopMe();
    });
}

void FileNetConnector::processDataFileProgress(FileDataSendProgress progress)
{

}
