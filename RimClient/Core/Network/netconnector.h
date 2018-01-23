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

#include <QMutex>
#include <QWaitCondition>

#include "Network/rtask.h"
#include "Network/rsocket.h"

namespace ClientNetwork
{
class MsgSender;
class MsgReceive;
}

namespace ClientNetwork {
class RTask;
}

class NetConnector : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    explicit NetConnector(QObject * parent = 0);
    ~NetConnector();

    static NetConnector * instance();

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

    void startMe();
    void stopMe();

signals:
    void connected(bool flag);

private slots:
    void respSocketError(int errorCode);

protected:
    void run();

private:
    void doConnect();
    void doReconnect();
    void doDisconnect();

private:
    static NetConnector * netConnector;

    ClientNetwork::RSocket * rsocket;

    NetCommand command;
    bool netConnected;

    int delayTime;

    QMutex mutex;
    QWaitCondition condition;

    ClientNetwork::MsgSender * msgSender;
    ClientNetwork::MsgReceive * msgReceive;
};

#endif // NETCONNECTOR_H
