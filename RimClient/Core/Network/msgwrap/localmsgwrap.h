/*!
 *  @brief     本地信息包装
 *  @details   实现不同的协议下的报文封装
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef LOCALMSGWRAP_H
#define LOCALMSGWRAP_H

#ifdef __LOCAL_CONTACT__

#include "../../protocol/datastruct.h"
using namespace ParameterSettings;

#include "msgwrap.h"

class LocalMsgWrap : public MsgWrap
{
public:
    LocalMsgWrap();

    void handleMsg(MsgPacket * packet,CommucationMethod method = C_None, MessageFormat format = M_NONE);
};

#endif

#endif // LOCALMSGWRAP_H
