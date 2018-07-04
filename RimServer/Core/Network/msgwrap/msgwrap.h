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
#include <QJsonArray>

#include "protocol/protocoldata.h"
using namespace ProtocolType;

#include "protocol/datastruct.h"
#include "Network/head.h"


class MsgWrap
{
public:
    MsgWrap();

    virtual void handleMsg(int sockId , MsgPacket * packet, int result = 0) = 0;
    virtual void hanldeMsgProtcol(int sockId,ProtocolPackage & package,bool inServer = true) = 0;

    virtual void handleMsgReply(int sockId ,MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1) = 0;
};

#endif // MSGWRAP_H
