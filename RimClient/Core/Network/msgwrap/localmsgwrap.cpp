#include "localmsgwrap.h"

#ifdef __LOCAL_CONTACT__

#include "../wraprule/udp_wraprule.h"
#include "../wraprule/tcp_wraprule.h"
#include "rsingleton.h"
#include "Network/netglobal.h"
#include "json_wrapformat.h"
#include "binary_wrapformat.h"
#include <QDebug>

LocalMsgWrap::LocalMsgWrap()
{

}

/*!
 * @brief 根据不同的通信方式与报文格式将数据进行格式装配，装配结束后压入发送栈
 * @param[in] method 通信方式
 * @param[in] format 报文格式
 * @param[in] packet 待发送的数据信息
 */
void LocalMsgWrap::handleMsg(MsgPacket * packet,CommucationMethod method, MessageFormat format)
{
    if(packet == nullptr)
        return;

    QByteArray result;
    switch(packet->msgType){
        case MSG_CONTROL:
        case MSG_TEXT:
            {
                RSingleton<Json_WrapFormat>::instance()->handleMsg(packet,result);
            }
            break;
        case MSG_FILE:
            {
                RSingleton<Binary_WrapFormat>::instance()->handleMsg(packet,result);
            }
            break;
        default:
            break;
    }

    //716
    if(packet->msgCommand == MsgCommand::MSG_TEXT_TEXT)
    {
        TextRequest *test = ((TextRequest *)packet);
        result = test->sendData.toLocal8Bit();
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
               <<""<<test->accountId<<test->otherSideId<<test->timeStamp
              <<"\n";

    }

    if(result.length() > 0){
        QByteArray sendResult;
        CommMethod commMethod = C_NONE;
        if(method == C_NetWork && format == M_205){
            commMethod = C_UDP;
            sendResult = RSingleton<UDP_WrapRule>::instance()->wrap(result);
        }else if(method == C_TongKong && format == M_495){
//            commMethod = C_TCP;
//            sendResult = RSingleton<TCP_WrapRule>::instance()->wrap(result);

            //716_兼容调试,暂时使用DDS+TCP报文格式发送数据，此处复用TCP发送格式
            commMethod = C_BUS;
            sendResult = RSingleton<TCP_WrapRule>::instance()->wrap(result);
        }
        if(commMethod != C_NONE){
            G_TextSendMutex.lock();
            G_TextSendBuffs.push({commMethod,sendResult});
            G_TextSendMutex.unlock();

            G_TextSendWaitCondition.notify_one();
        }
    }
}

#endif
