/*!
 *  @brief     基本数据包装
 *  @details   用于Client-Server之间的信息协议传输
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.08
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef BASEMSGWRAP_H
#define BASEMSGWRAP_H

#include "../../protocol/protocoldata.h"
using namespace ProtocolType;

#include "msgwrap.h"

class BaseMsgWrap : public MsgWrap
{
public:
    BaseMsgWrap();

    void handleMsg(int sockId , MsgPacket * packet, int result = 0);
    void hanldeMsgProtol(int sockId,ProtocolPackage & package){}

    void handleMsgReply(int sockId ,MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1);
};

#endif // BASEMSGWRAP_H
