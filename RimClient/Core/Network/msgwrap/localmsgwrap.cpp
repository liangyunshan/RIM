#include "localmsgwrap.h"

#ifdef __LOCAL_CONTACT__

#include "util/rsingleton.h"
#include "Network/netglobal.h"
#include "json_wrapformat.h"
#include "binary_wrapformat.h"
#include "global.h"
#include "user/user.h"
#include "others/serialno.h"
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
void LocalMsgWrap::handleMsg(MsgPacket * packet, CommucationMethod method, MessageFormat format, ServerType stype)
{
    if(packet == nullptr)
        return;

    SendUnit unit;
    {
        unit.dataUnit.cDate = QDate::currentDate().toString("yyyyMMdd").toInt();
        unit.dataUnit.cTime = QTime::currentTime().toString("hhmmss").toUInt();
    }

    switch (packet->msgCommand)
    {
    case MsgCommand::MSG_TEXT_TEXT:
        {
            TextRequest *textRequest = dynamic_cast<TextRequest *>(packet);
            if(textRequest)
            {
                unit.dataUnit.wSourceAddr = textRequest->accountId.toInt();
                unit.dataUnit.wDestAddr = textRequest->otherSideId.toInt();
                unit.dataUnit.data = textRequest->sendData.toLocal8Bit();
                unit.dataUnit.bPackType = T_DATA_AFFIRM;
                unit.dataUnit.bPeserve = 0;
                unit.dataUnit.usSerialNo = textRequest->textId.toInt();
                unit.dataUnit.usOrderNo = O_2051;
            }
        }
        break;
    case MsgCommand::MSG_USER_HISTORY_MSG:
        {
            HistoryMessRequest *textRequest = dynamic_cast<HistoryMessRequest *>(packet);
            if(textRequest)
            {
                unit.dataUnit.wSourceAddr = textRequest->accountId.toInt();
                unit.dataUnit.wDestAddr = textRequest->accountId.toInt();
                unit.dataUnit.usSerialNo = SERIALNO_FRASH;
                unit.dataUnit.bPackType = T_DATA_NOAFFIRM;
                unit.dataUnit.bPeserve = 0X80;
            }
        }
        break;
    case MsgCommand::MSG_TCP_TRANS:
        {
            DataPackType *dataPackType = dynamic_cast<DataPackType *>(packet);
            if(dataPackType)
            {
                unit.dataUnit.wSourceAddr = dataPackType->sourceId.toInt();
                unit.dataUnit.wDestAddr = dataPackType->destId.toInt();
                unit.dataUnit.bPackType = dataPackType->extendData.type495;
                unit.dataUnit.usOrderNo = dataPackType->extendData.usOrderNo;
                if(dataPackType->extendData.usSerialNo == 0)
                {
                    unit.dataUnit.usSerialNo = SERIALNO_FRASH;
                }
                else
                {
                    unit.dataUnit.usSerialNo = dataPackType->extendData.usSerialNo;
                }
                unit.dataUnit.data = dataPackType->extendData.data;
                unit.dataUnit.bPeserve = 0;
            }
        }
        break;
    case MsgCommand::MSG_TEXT_FILE:
        {
        }
        break;
    default:
        break;
    }

    if(method == C_NetWork && format == M_205){
        unit.method = C_UDP;
    }else if(method == C_TongKong && format == M_495){
        unit.method = C_TCP;
    }

    SerialNo::instance()->updateSqlSerialNo(unit.dataUnit.usSerialNo);

    if(stype == SERVER_TEXT){
        if(unit.method != C_NONE){
            G_TextSendMutex.lock();
            G_TextSendBuffs.push(unit);
            G_TextSendMutex.unlock();
            G_TextSendWaitCondition.notify_one();
        }
    }else if(stype == SERVER_FILE){
        if(unit.method != C_NONE){
            G_FileSendMutex.lock();
            G_FileSendBuffs.push(unit);
            G_FileSendMutex.unlock();
            G_FileSendWaitCondition.notify_one();
        }
    }
}

#endif
