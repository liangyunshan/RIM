#include "msgwrap.h"

#include <QJsonDocument>
#include <QMutexLocker>
#include <QDebug>
#include <QMetaEnum>

#include "Network/netglobal.h"
#include "Util/rlog.h"
#include "jsonkey.h"

MsgWrap::MsgWrap()
{

}

/*!
     * @brief 为普通信息添加信息头
     * @param[in] packet 待发送原始数据包信息
     * @return 添加信息头后的数据信息
     */
QByteArray MsgWrap::handleMsg(MsgPacket *packet)
{
    if(packet == NULL)
    {
        return QByteArray();
    }

    switch(packet->msgCommand)
    {
        case MsgCommand::MSG_USER_REGISTER:
                                            return handleRegistResponse((RegistResponse *)packet);
        case MsgCommand::MSG_USER_LOGIN:
                                            return handleLoginResponse((LoginResponse *)packet);
        case MsgCommand::MSG_USER_UPDATE_INFO:
                                            return handleUpdateBaseInfoResponse((UpdateBaseInfoResponse *)packet);
        default:
                break;
    }

    return QByteArray();
}

/*!
     * @brief 处理简单错误信息
     * @param[in] type 信息类型
     * @param[in] command 命令类型
     * @param[in] errorCode 错误码(客户端可根据信息类型-命令类型-错误码定义具体的错误信息)
     * @return 序列化数据
     */
QByteArray MsgWrap::handleErrorSimpleMsg(MsgType type,MsgCommand command, int errorCode)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Type),type);
    obj.insert(JsonKey::key(JsonKey::Command),command);

    obj.insert(JsonKey::key(JsonKey::Status),errorCode);

    QJsonDocument document;
    document.setObject(obj);

    return document.toJson(QJsonDocument::Compact);
}

QByteArray MsgWrap::handleRegistResponse(RegistResponse * packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);

    return wrappedPack(packet,data);
}

//处理用户登陆
QByteArray MsgWrap::handleLoginResponse(LoginResponse *packet)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::AccountId),packet->baseInfo.accountId);
    data.insert(JsonKey::key(JsonKey::NickName),packet->baseInfo.nickName);
    data.insert(JsonKey::key(JsonKey::SignName),packet->baseInfo.signName);
    data.insert(JsonKey::key(JsonKey::Sexual),packet->baseInfo.sexual);
    data.insert(JsonKey::key(JsonKey::Birth),packet->baseInfo.birthday);
    data.insert(JsonKey::key(JsonKey::Address),packet->baseInfo.address);
    data.insert(JsonKey::key(JsonKey::Email),packet->baseInfo.email);
    data.insert(JsonKey::key(JsonKey::Phone),packet->baseInfo.phoneNumber);
    data.insert(JsonKey::key(JsonKey::Desc),packet->baseInfo.desc);
    data.insert(JsonKey::key(JsonKey::Face),packet->baseInfo.face);
    data.insert(JsonKey::key(JsonKey::FaceId),packet->baseInfo.customImgId);

    return wrappedPack(packet,data);
}

//处理更新用户信息
QByteArray MsgWrap::handleUpdateBaseInfoResponse(UpdateBaseInfoResponse * packet)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::AccountId),packet->baseInfo.accountId);
    data.insert(JsonKey::key(JsonKey::NickName),packet->baseInfo.nickName);
    data.insert(JsonKey::key(JsonKey::SignName),packet->baseInfo.signName);
    data.insert(JsonKey::key(JsonKey::Sexual),packet->baseInfo.sexual);
    data.insert(JsonKey::key(JsonKey::Birth),packet->baseInfo.birthday);
    data.insert(JsonKey::key(JsonKey::Address),packet->baseInfo.address);
    data.insert(JsonKey::key(JsonKey::Email),packet->baseInfo.email);
    data.insert(JsonKey::key(JsonKey::Phone),packet->baseInfo.phoneNumber);
    data.insert(JsonKey::key(JsonKey::Desc),packet->baseInfo.desc);
    data.insert(JsonKey::key(JsonKey::Face),packet->baseInfo.face);
    data.insert(JsonKey::key(JsonKey::FaceId),packet->baseInfo.customImgId);

    return wrappedPack(packet,data);
}

/*!
     * @brief 为正确信息添加信息头
     * @param[in] packet
     * @return 是否插入成功
     */
QByteArray MsgWrap::wrappedPack(MsgPacket *packet,QJsonObject & data)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Type),packet->msgType);
    obj.insert(JsonKey::key(JsonKey::Command),packet->msgCommand);
    obj.insert(JsonKey::key(JsonKey::Status),0);

    obj.insert(JsonKey::key(JsonKey::Data),data);

    QJsonDocument document;
    document.setObject(obj);

    return document.toJson(QJsonDocument::Compact);
}
