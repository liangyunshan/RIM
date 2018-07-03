#include "localmsgwrap.h"

#ifdef __LOCAL_CONTACT__

#include "../wraprule/udp_wraprule.h"
#include "../wraprule/tcp_wraprule.h"
#include "../wraprule/qdb21_wraprule.h"
#include "../wraprule/qdb2051_wraprule.h"
#include "rsingleton.h"
#include "Network/netglobal.h"
#include "json_wrapformat.h"
#include "binary_wrapformat.h"
#include "global.h"
#include "user/user.h"
#include <QDebug>

LocalMsgWrap::LocalMsgWrap()
{

}

/*!
 * @brief 根据不同的通信方式与报文格式将数据进行格式装配，装配结束后压入发送栈
 * @param[in] packet 待发送的数据信息
 * @param[in] method 通信方式
 * @param[in] format 报文格式
 */
void LocalMsgWrap::handleMsg(MsgPacket * packet,CommucationMethod method, MessageFormat format)
{
    if(packet == nullptr)
        return;

    ProtocolPackage package;
    CommMethod commMethod = C_NONE;
    switch (packet->msgCommand)
    {
    case MsgCommand::MSG_TEXT_TEXT:
        {
            TextRequest *textRequest = dynamic_cast<TextRequest *>(packet);
            if(textRequest)
            {
                package.wSourceAddr = textRequest->accountId.toInt();
                package.wDestAddr = textRequest->otherSideId.toInt();
                package.data = textRequest->sendData.toLocal8Bit();
                package.bPackType = T_DATA_AFFIRM;
                package.bPeserve = 0;
                package.usSerialNo = textRequest->textId.toInt();
                package.usOrderNo = 2051;

                method = C_TongKong ;
                format = M_495 ;
            }
        }
        break;
    case MsgCommand::MSG_USER_HISTORY_MSG:
        {
            HistoryMessRequest *textRequest = dynamic_cast<HistoryMessRequest *>(packet);
            if(textRequest)
            {
                package.wSourceAddr = textRequest->accountId.toInt();
                package.wDestAddr = textRequest->accountId.toInt();
                package.bPackType = T_DATA_NOAFFIRM;
                package.bPeserve = 0X80;

                method = C_TongKong ;
                format = M_495 ;
            }
        }
        break;
    case MsgCommand::MSG_TCP_TRANS:
        {
            DataPackType *dataPackType = dynamic_cast<DataPackType *>(packet);
            if(dataPackType)
            {
                package.wSourceAddr = dataPackType->accountId.toInt();
                package.wDestAddr = dataPackType->accountId.toInt();
                package.bPackType = T_DATA_REG;
                package.bPeserve = 0X80;

                method = C_TongKong ;
                format = M_495 ;
            }
        }
        break;
    default:
        break;
    }

    if(method == C_NetWork && format == M_205){
        commMethod = C_UDP;
        RSingleton<UDP_WrapRule>::instance()->wrap(package);
    }else if(method == C_TongKong && format == M_495){
        commMethod = C_TCP;
        RSingleton<TCP_WrapRule>::instance()->wrap(package);
    }
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
           <<"package.usSerialNo:"<<package.usSerialNo
          <<"\n";
    if(commMethod != C_NONE){
        SendUnit unit;
        unit.method = commMethod;
        unit.dataUnit = package;

        G_TextSendMutex.lock();
        G_TextSendBuffs.push(unit);
        G_TextSendMutex.unlock();
        G_TextSendWaitCondition.notify_one();
    }
}

#endif
