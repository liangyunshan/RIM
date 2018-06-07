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

#include <QJsonObject>

#include "datastruct.h"
using namespace ParameterSettings;

#include "protocoldata.h"
using namespace ProtocolType;

class LocalMsgWrap
{
public:
    LocalMsgWrap();

    void wrapMessage(const CommucationMethod & method, const MessageFormat & format, MsgPacket * packet);

private:
    void handleTextRequest(TextRequest *packet, QByteArray &result);

    void wrappedPack(MsgPacket *packet,QJsonObject & data,QByteArray& result);

};

#endif

#endif // LOCALMSGWRAP_H
