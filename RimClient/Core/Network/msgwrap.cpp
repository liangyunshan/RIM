#include "msgwrap.h"

#include <QJsonDocument>
#include <QMutexLocker>
#include <QDebug>

#include "Network/netglobal.h"
#include "Util/rlog.h"
#include "jsonkey.h"

MsgWrap::MsgWrap()
{

}

void MsgWrap::handleMsg(MsgPacket *packet)
{
    if(packet == NULL)
    {
        return;
    }

    switch(packet->msgCommand)
    {
        case MsgCommand::MSG_USER_REGISTER:
                                            handleRegistRequest((RegistRequest *)packet);
                                            break;
        case MsgCommand::MSG_USER_LOGIN:
                                            handleLoginRequest((LoginRequest *)packet);
                                            break;
        default:
                break;
    }

}

//处理用户登陆
void MsgWrap::handleLoginRequest(LoginRequest *packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::Pass),packet->password);
    data.insert(JsonKey::key(JsonKey::Status),packet->status);

    wrappedPack(packet,data);
}

//处理用户注册
void MsgWrap::handleRegistRequest(RegistRequest *packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::NickName),packet->nickName);
    data.insert(JsonKey::key(JsonKey::Pass),packet->password);

    wrappedPack(packet,data);
}

void MsgWrap::wrappedPack(MsgPacket *packet,QJsonObject & data)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Type),packet->msgType);
    obj.insert(JsonKey::key(JsonKey::Command),packet->msgCommand);

    obj.insert(JsonKey::key(JsonKey::Data),data);

    QJsonDocument document;
    document.setObject(obj);
    QByteArray array = document.toJson(QJsonDocument::Compact);

    delete packet;

    G_SendMutex.lock();
    G_SendBuff.enqueue(array);
    G_SendMutex.unlock();

    G_SendWaitCondition.wakeOne();

    RLOG_INFO("Send msg:%s",array.data());
}
