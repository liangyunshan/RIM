/*!
 *  @brief     本地信息包装
 *  @details   实现716通信协议下的报文封装
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.28
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note       20180702:wey:增加服务器-服务器数据封装
 */
#ifndef LOCALMSGWRAP_H
#define LOCALMSGWRAP_H

#ifdef __LOCAL_CONTACT__

#include <QString>

#include "../../protocol/protocoldata.h"
using namespace ProtocolType;

#include "msgwrap.h"

class NetConnector;

class LocalMsgWrap : public MsgWrap
{
public:
    LocalMsgWrap();

    void handleMsg(int sockId ,MsgPacket * packet, int result = 0){}
    void hanldeMsgProtcol(int sockId, ProtocolPackage & package,bool inServer = true);

    void handleMsgReply(int sockId ,MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1);
};

#endif

#endif // LOCALMSGWRAP_H
