/*!
 *  @brief     本地信息包装
 *  @details   实现716通信协议下的报文封装
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef LOCALMSGWRAP_H
#define LOCALMSGWRAP_H

#ifdef __LOCAL_CONTACT__

#include "../../protocol/protocoldata.h"
using namespace ProtocolType;

#include "msgwrap.h"

class LocalMsgWrap : public MsgWrap
{
public:
    LocalMsgWrap();

    void hanldeMsgProtol(int sockId, ProtocolPackage & package);
    void handleMsg(int sockId ,MsgPacket * packet, int result = 0){}

    void handleMsgReply(int sockId ,MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1);
};

#endif

#endif // LOCALMSGWRAP_H
