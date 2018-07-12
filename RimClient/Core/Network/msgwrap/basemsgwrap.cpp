#include "basemsgwrap.h"

#include "json_wrapformat.h"
#include "binary_wrapformat.h"
#include "Network/netglobal.h"
#include "util/rsingleton.h"

BaseMsgWrap::BaseMsgWrap()
{

}

void BaseMsgWrap::handleMsg(MsgPacket * packet
#ifdef __LOCAL_CONTACT__
                        , CommucationMethod method, MessageFormat format
#endif
   )
{
#ifdef __LOCAL_CONTACT__
    Q_UNUSED(method)
    Q_UNUSED(format)
#endif

    QByteArray result;
    switch(packet->msgType){
        case MSG_CONTROL:
        case MSG_TEXT:
            {
                RSingleton<Json_WrapFormat>::instance()->handleMsg(packet,result);
                G_TextSendMutex.lock();
                G_TextSendBuffs.push({C_TCP,result});
                G_TextSendMutex.unlock();

                G_TextSendWaitCondition.notify_one();
            }
            break;
        case MSG_FILE:
            {
                RSingleton<Binary_WrapFormat>::instance()->handleMsg(packet,result);

                G_FileSendMutex.lock();
                G_FileSendBuffs.push({C_TCP,result});
                G_FileSendMutex.unlock();

                G_FileSendWaitCondition.notify_one();
            }
            break;
        default:
            break;
    }
}
