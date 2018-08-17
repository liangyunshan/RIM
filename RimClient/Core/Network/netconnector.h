/*!
 *  @brief     网络连接
 *  @details   管理网络连接，包括创建网络、获取网络状态、关闭网络连接等
 *  @file      netconnector.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20180119:wey:将连接和重连改成异步，降低页面阻塞。
 */
#ifndef NETCONNECTOR_H
#define NETCONNECTOR_H

#include "Network/rtask.h"
#include "Network/rsocket.h"
#include <mutex>
#include <condition_variable>
#include <memory>
#include <map>
#include <vector>

namespace ClientNetwork
{
class TextSender;
class FileSender;
class DataRecveiveTask;
}

namespace ClientNetwork {
class RTask;
class TcpTransmit;
class BaseTransmit;
}

class SuperConnector : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    explicit SuperConnector(QObject * parent = 0);
    virtual ~SuperConnector();
    enum NetCommand
    {
        Net_None,
        Net_Connect,
        Net_Reconnect,
        Net_Disconnect,
        Net_Heart
    };

    virtual bool initialize() = 0;
    void connect(int delaySecTime = 3);            //延迟秒
    void reconnect(int delaySecTime = 3);
    void disConnect();

    bool isSockValid(){return rsocket->isValid();}
    bool isConnected()const {return netConnected;}

    ClientNetwork::RSocket * socket(){return rsocket;}

    void startMe();
    void stopMe();

protected:
    virtual void doConnect()=0;
    virtual void doReconnect()=0;
    virtual void doDisconnect()=0;

    void run();

    void handleTextOnly(RecvUnit &data);
    void handleTextAndFile(RecvUnit &data);
    void handleFileOnly(RecvUnit &data);

protected slots:
    virtual void respSocketError(CommMethod method)=0;

protected:
    ClientNetwork::RSocket * rsocket;

    NetCommand command;
    bool netConnected;

    QString remoteIp;
    unsigned short remotePort;

    int delayTime;

    std::mutex mutex;
    std::condition_variable condition;
    std::map<CommMethod,std::shared_ptr<ClientNetwork::BaseTransmit>> transmits;
};

class TextNetConnector : public SuperConnector
{
    Q_OBJECT
public:
    explicit TextNetConnector();
    ~TextNetConnector();

    static TextNetConnector * instance();

    bool initialize();

private:
    void doConnect();
    void doReconnect();
    void doDisconnect();

protected slots:
    void respSocketError(CommMethod method);

private:
    static TextNetConnector * netConnector;

    std::shared_ptr<ClientNetwork::TextSender> msgSender;
    std::map<CommMethod,std::shared_ptr<ClientNetwork::DataRecveiveTask>> msgReceives;

};

#ifndef __LOCAL_CONTACT__
class FileNetConnector : public SuperConnector
{
    Q_OBJECT
public:
    explicit FileNetConnector();
    ~FileNetConnector();

    static FileNetConnector * instance();

    bool initialize();

private slots:
    void respSocketError(CommMethod method);

private:
    void doConnect();
    void doReconnect();
    void doDisconnect();

    void processDataFileProgress(FileDataSendProgress progress);
private:
    static FileNetConnector * netConnector;

    std::shared_ptr<ClientNetwork::FileSender> msgSender;
    std::map<CommMethod,std::shared_ptr<ClientNetwork::DataRecveiveTask>> msgReceives;
};

#endif

#endif // NETCONNECTOR_H
