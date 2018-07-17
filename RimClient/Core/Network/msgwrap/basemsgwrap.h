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

    void handleMsg(MsgPacket * packet
#ifdef __LOCAL_CONTACT__
                   , CommucationMethod method = C_None, MessageFormat format = M_NONE,ServerType stype = SERVER_TEXT
#endif
            );

};

#endif // BASEMSGWRAP_H
