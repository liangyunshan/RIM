﻿/*!
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

#include <QMutex>
#include <QWaitCondition>

#include "Network/rtask.h"
#include "Network/rsocket.h"

namespace ClientNetwork
{
class TextSender;
class FileSender;
class TextReceive;
class FileReceive;
}

namespace ClientNetwork {
class RTask;
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

    void connect(int delayTime = 3);            //延迟秒
    void reconnect(int delayTime = 3);
    void disConnect();

    bool setBlock(bool flag);
    bool isBock(){return rsocket->isBock();}

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

protected slots:
    virtual void respSocketError(int errorCode)=0;

protected:
    ClientNetwork::RSocket * rsocket;

    NetCommand command;
    bool netConnected;

    int delayTime;

    QMutex mutex;
    QWaitCondition condition;

};

class TextNetConnector : public SuperConnector
{
    Q_OBJECT
public:
    explicit TextNetConnector();
    ~TextNetConnector();

    static TextNetConnector * instance();

private:
    void doConnect();
    void doReconnect();
    void doDisconnect();

protected slots:
    void respSocketError(int errorCode);

private:
    static TextNetConnector * netConnector;

    ClientNetwork::TextSender * msgSender;
    ClientNetwork::TextReceive * msgReceive;
};

class FileNetConnector : public SuperConnector
{
    Q_OBJECT
public:
    explicit FileNetConnector();
    ~FileNetConnector();

    static FileNetConnector * instance();

private slots:
    void respSocketError(int errorCode);

private:
    void doConnect();
    void doReconnect();
    void doDisconnect();
private:
    static FileNetConnector * netConnector;

    ClientNetwork::FileSender * msgSender;
    ClientNetwork::FileReceive * msgReceive;
};

#endif // NETCONNECTOR_H
