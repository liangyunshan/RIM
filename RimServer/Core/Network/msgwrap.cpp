#include "msgwrap.h"

#include <QJsonDocument>
#include <QMutexLocker>
#include <QDebug>

#include "Network/netglobal.h"
#include "Util/rlog.h"

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
        case MsgCommand::MSG_USER_LOGIN:
                                            return handleLoginResponse((LoginResponse *)packet);
        default:
                break;
    }
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
    obj.insert(context.msgType,type);
    obj.insert(context.msgCommand,command);

    obj.insert(context.msgStatus,QJsonValue(errorCode));

    QJsonDocument document;
    document.setObject(obj);

    return document.toJson(QJsonDocument::Compact);
}

//处理用户登陆
QByteArray MsgWrap::handleLoginResponse(LoginResponse *packet)
{
    QJsonObject data;
    data.insert("accountName",packet->accountName);
    data.insert("nickName",packet->nickName);
    data.insert("signName",packet->signName);
    data.insert("sexual",packet->sexual);
    data.insert("birthday",packet->birthday);
    data.insert("address",packet->address);
    data.insert("email",packet->email);
    data.insert("phoneNumber",packet->phoneNumber);
    data.insert("desc",packet->desc);
    data.insert("face",packet->face);

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
    obj.insert(context.msgType,packet->msgType);
    obj.insert(context.msgCommand,packet->msgCommand);

    obj.insert("data",data);

    QJsonDocument document;
    document.setObject(obj);

    return document.toJson(QJsonDocument::Compact);
}
