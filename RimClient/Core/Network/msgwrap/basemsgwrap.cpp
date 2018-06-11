#include "basemsgwrap.h"

#include "json_wrapformat.h"
#include "binary_wrapformat.h"
#include "Network/netglobal.h"
#include "rsingleton.h"

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
                G_TextSendBuffs.enqueue({C_TCP,result});
                G_TextSendMutex.unlock();

                G_TextSendWaitCondition.wakeOne();
            }
            break;
        case MSG_FILE:
            {
                RSingleton<Binary_WrapFormat>::instance()->handleMsg(packet,result);

                G_FileSendMutex.lock();
                G_FileSendBuffs.enqueue({C_TCP,result});
                G_FileSendMutex.unlock();

                G_FileSendWaitCondition.wakeOne();
            }
            break;
        default:
            break;
    }
}
