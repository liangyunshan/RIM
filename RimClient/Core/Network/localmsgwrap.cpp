#include "localmsgwrap.h"

#ifdef __LOCAL_CONTACT__

#include <QJsonDocument>

#include "udp_wraprule.h"
#include "tcp_wraprule.h"
#include "rsingleton.h"
#include "jsonkey.h"
#include "Network/netglobal.h"

LocalMsgWrap::LocalMsgWrap()
{

}

/*!
 * @brief 根据不同的通信方式与报文格式将数据进行格式装配，装配结束后压入发送栈
 * @param[in] method 通信方式
 * @param[in] format 报文格式
 * @param[in] packet 待发送的数据信息
 */
void LocalMsgWrap::wrapMessage(const CommucationMethod &method, const MessageFormat &format, MsgPacket *packet)
{
    if(packet == nullptr)
        return;

    QByteArray result;

    switch(packet->msgCommand){
        case MsgCommand::MSG_TEXT_TEXT:
            handleTextRequest((TextRequest *)packet,result);
            break;
        default:
            break;
    }

    if(result.length() > 0){
        QByteArray sendResult;
        CommMethod commMethod = C_UDP;
        if(method == C_NetWork && format == M_205){
            commMethod = C_UDP;
            sendResult = RSingleton<UDP_WrapRule>::instance()->wrap(result);
        }else if(method == C_TongKong && format == M_495){
            commMethod = C_TCP;
            sendResult = RSingleton<TCP_WrapRule>::instance()->wrap(result);
        }

        G_TextSendMutex.lock();
        G_TextSendBuffs.enqueue({commMethod,sendResult});
        G_TextSendMutex.unlock();

        G_TextSendWaitCondition.wakeOne();
    }
}

/*!
 * @brief 包装文字信息请求
 * @param[in] packet 文件请求内容
 * @param[in/out] result 结果信息
 */
void LocalMsgWrap::handleTextRequest(TextRequest *packet,QByteArray& result)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::TextId),packet->textId);
    data.insert(JsonKey::key(JsonKey::Time),packet->timeStamp);
    data.insert(JsonKey::key(JsonKey::Type),packet->textType);
    data.insert(JsonKey::key(JsonKey::Encryption),packet->isEncryption);
    data.insert(JsonKey::key(JsonKey::Compress),packet->isCompress);
    data.insert(JsonKey::key(JsonKey::OperateType),packet->type);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::OtherSideId),packet->otherSideId);
    data.insert(JsonKey::key(JsonKey::Data),packet->sendData);

    wrappedPack(packet,data,result);
}

void LocalMsgWrap::wrappedPack(MsgPacket *packet,QJsonObject & data,QByteArray& result)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Type),packet->msgType);
    obj.insert(JsonKey::key(JsonKey::Command),packet->msgCommand);

    obj.insert(JsonKey::key(JsonKey::Data),data);

    QJsonDocument document;
    document.setObject(obj);
    result = document.toJson(QJsonDocument::Compact);

    if(packet->isAutoDelete)
        delete packet;
}

#endif
