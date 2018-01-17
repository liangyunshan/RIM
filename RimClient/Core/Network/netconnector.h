/*!
 *  @brief     网络连接
 *  @details   管理网络连接，包括创建网络、获取网络状态、关闭网络连接等
 *  @file      netconnector.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.15
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef NETCONNECTOR_H
#define NETCONNECTOR_H

#include <QString>

#include "Network/rsocket.h"
using namespace ClientNetwork;

class NetConnector
{
public:
    NetConnector();

    bool connect(int delayMillSecond = 3);
    bool reconnect(int delayMillSecond = 3);

    bool setBlock(bool flag);
    bool isBock(){return socket.isBock();}

    bool isValid(){return socket.isValid();}

private:
    RSocket socket;

    bool connected;
};

#endif // NETCONNECTOR_H
