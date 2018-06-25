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
using namespace ParameterSettings;

class MsgWrap
{
public:
    MsgWrap();

    virtual QByteArray handleMsg(MsgPacket * packet, int result = 0
#ifdef __LOCAL_CONTACT__
                           ,CommucationMethod method = C_None,MessageFormat format = M_NONE
#endif
            ) = 0;

    virtual QByteArray handleMsgReply(MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1) = 0;
};

#endif // MSGWRAP_H
