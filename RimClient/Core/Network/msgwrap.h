/*!
 *  @brief     网络数据转换
 *  @details   接收应用层的数据，将其转换为网络数据发送结构
 *  @file      msgwrap.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.15
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGWRAP_H
#define MSGWRAP_H

#include <QJsonObject>

#include "msgcontext.h"
#include "protocoldata.h"
using namespace ProtocolType;

class MsgWrap
{
public:
    MsgWrap();

    void handleMsg(MsgPacket * packet);

private:
    void handleRegistRequest(RegistRequest *packet);
    void handleLoginRequest(LoginRequest *packet);

    void wrappedPack(MsgPacket * packet, QJsonObject &data);

private:
    MsgRequestContext context;
};

#endif // MSGWRAP_H
