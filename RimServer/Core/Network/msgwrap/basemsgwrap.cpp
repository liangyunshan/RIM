#include "basemsgwrap.h"

#include "rsingleton.h"
#include "json_wrapformat.h"
#include "binary_wrapformat.h"
#include "Network/head.h"
#include "Network/netglobal.h"

BaseMsgWrap::BaseMsgWrap():MsgWrap()
{

}

void BaseMsgWrap::handleMsg(int sockId ,MsgPacket *packet, int result
#ifdef __LOCAL_CONTACT__
        ,CommucationMethod method,MessageFormat format
#endif
   )
{
#ifdef __LOCAL_CONTACT__
    Q_UNUSED(method)
    Q_UNUSED(format)
#endif

    SendUnit data;
    data.sockId = sockId;

    switch(packet->msgType){
        case MSG_CONTROL:
        case MSG_TEXT:
            {
                data.dataUnit.data = RSingleton<Json_WrapFormat>::instance()->handleMsg(packet,result);
            }
            break;
        case MSG_FILE:
            {
                data.dataUnit.data = RSingleton<Binary_WrapFormat>::instance()->handleMsg(packet,result);
            }
            break;
        default:
            break;
    }

    std::unique_lock<std::mutex> ul(G_SendMutex);
    G_SendButts.push(data);
    G_SendCondition.notify_one();
}

void BaseMsgWrap::handleMsgReply(int sockId ,MsgType type, MsgCommand command, int replyCode, int subMsgCommand)
{
    SendUnit data;
    data.sockId = sockId;

    data.dataUnit.data = RSingleton<Json_WrapFormat>::instance()->handleMsgReply(type,command,replyCode,subMsgCommand);

    std::unique_lock<std::mutex> ul(G_SendMutex);
    G_SendButts.push(data);
    G_SendCondition.notify_one();
}
