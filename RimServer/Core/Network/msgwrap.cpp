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
        case MsgCommand::MSG_RELATION_SEARCH:
                                            return handleSearchFriendResponse((SearchFriendResponse *)packet);
        case MsgCommand::MSG_RELATION_OPERATE:
                                            return handleOperateFriendResponse((OperateFriendResponse *)packet);
        case MsgCommand::MSG_RELATION_LIST:
                                            return handleFriendListResponse((FriendListResponse *)packet);
        case MsgCommand::MSG_GROUPING_OPERATE:
                                            return handleGroupingResponse((GroupingResponse *)packet);

        default:
                break;
    }

    return QByteArray();
}

QByteArray MsgWrap::handleText(TextResponse *response)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::AccountId),response->accountId);
    obj.insert(JsonKey::key(JsonKey::Type),response->type);
    obj.insert(JsonKey::key(JsonKey::Data),response->sendData);
    obj.insert(JsonKey::key(JsonKey::Time),response->timeStamp);
    obj.insert(JsonKey::key(JsonKey::FromId),response->fromAccountId);

    return wrappedPack(response,STATUS_SUCCESS,obj);
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

    return wrappedPack(packet,REGISTER_SUCCESS,data);
}

//处理用户登陆
QByteArray MsgWrap::handleLoginResponse(LoginResponse *packet)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::Uuid),packet->baseInfo.uuid);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->baseInfo.accountId);
    data.insert(JsonKey::key(JsonKey::NickName),packet->baseInfo.nickName);
    data.insert(JsonKey::key(JsonKey::SignName),packet->baseInfo.signName);
    data.insert(JsonKey::key(JsonKey::Sexual),packet->baseInfo.sexual);
    data.insert(JsonKey::key(JsonKey::Birth),packet->baseInfo.birthday);
    data.insert(JsonKey::key(JsonKey::Address),packet->baseInfo.address);
    data.insert(JsonKey::key(JsonKey::Email),packet->baseInfo.email);
    data.insert(JsonKey::key(JsonKey::Phone),packet->baseInfo.phoneNumber);
    data.insert(JsonKey::key(JsonKey::Remark),packet->baseInfo.remark);
    data.insert(JsonKey::key(JsonKey::Face),packet->baseInfo.face);
    data.insert(JsonKey::key(JsonKey::FaceId),packet->baseInfo.customImgId);

    return wrappedPack(packet,LOGIN_SUCCESS,data);
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
    data.insert(JsonKey::key(JsonKey::Remark),packet->baseInfo.remark);
    data.insert(JsonKey::key(JsonKey::Face),packet->baseInfo.face);
    data.insert(JsonKey::key(JsonKey::FaceId),packet->baseInfo.customImgId);

    return wrappedPack(packet,UPDATE_USER_SUCCESS,data);
}

QByteArray MsgWrap::handleSearchFriendResponse(SearchFriendResponse * packet)
{
    QJsonArray data;

    for(int i = 0; i < packet->result.size(); i++)
    {
        QJsonObject obj;
        obj.insert(JsonKey::key(JsonKey::AccountId),packet->result.at(i).accountId);
        obj.insert(JsonKey::key(JsonKey::NickName),packet->result.at(i).nickName);
        obj.insert(JsonKey::key(JsonKey::SignName),packet->result.at(i).signName);
        obj.insert(JsonKey::key(JsonKey::Face),packet->result.at(i).face);
        obj.insert(JsonKey::key(JsonKey::FaceId),packet->result.at(i).customImgId);

        data.append(obj);
    }

    return wrappedPack(packet,FIND_FRIEND_FOUND,data);
}

QByteArray MsgWrap::handleOperateFriendResponse(OperateFriendResponse * packet)
{
    QJsonObject obj;

    obj.insert(JsonKey::key(JsonKey::Type),packet->type);
    obj.insert(JsonKey::key(JsonKey::Result),packet->result);
    obj.insert(JsonKey::key(JsonKey::SearchType),packet->stype);
    obj.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);

    QJsonObject requsetInfo;
    requsetInfo.insert(JsonKey::key(JsonKey::AccountId),packet->requestInfo.accountId);
    requsetInfo.insert(JsonKey::key(JsonKey::NickName),packet->requestInfo.nickName);
    requsetInfo.insert(JsonKey::key(JsonKey::SignName),packet->requestInfo.signName);
    requsetInfo.insert(JsonKey::key(JsonKey::Face),packet->requestInfo.face);
    requsetInfo.insert(JsonKey::key(JsonKey::FaceId),packet->requestInfo.customImgId);

    obj.insert(JsonKey::key(JsonKey::OperateInfo),requsetInfo);

    return wrappedPack(packet,FRIEND_REQUEST,obj);
}

QByteArray MsgWrap::handleFriendListResponse(FriendListResponse *packet)
{
    QJsonObject obj;

    obj.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);

    QJsonArray groups;
    for(int i  = 0; i < packet->groups.size(); i++)
    {
        RGroupData * gdata = packet->groups.at(i);

        QJsonObject groupData;
        groupData.insert(JsonKey::key(JsonKey::GroupId),gdata->groupId);
        groupData.insert(JsonKey::key(JsonKey::GroupName),gdata->groupName);
        groupData.insert(JsonKey::key(JsonKey::IsDefault),gdata->isDefault);

        QJsonArray users;
        for(int j = 0; j < gdata->users.size(); j++)
        {
            SimpleUserInfo userInfo = gdata->users.at(j);

            QJsonObject user;
            user.insert(JsonKey::key(JsonKey::AccountId),userInfo.accountId);
            user.insert(JsonKey::key(JsonKey::NickName),userInfo.nickName);
            user.insert(JsonKey::key(JsonKey::SignName),userInfo.signName);
            user.insert(JsonKey::key(JsonKey::Face),userInfo.face);
            user.insert(JsonKey::key(JsonKey::FaceId),userInfo.customImgId);
            user.insert(JsonKey::key(JsonKey::Remark),userInfo.remarks);

            users.append(user);
        }
        groupData.insert(JsonKey::key(JsonKey::Users),users);

        groups.append(groupData);
    }
    obj.insert(JsonKey::key(JsonKey::Groups),groups);

    return wrappedPack(packet,STATUS_SUCCESS,obj);
}

QByteArray MsgWrap::handleGroupingResponse(GroupingResponse *packet)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Uuid),packet->uuid);
    obj.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    obj.insert(JsonKey::key(JsonKey::GroupType),packet->gtype);
    obj.insert(JsonKey::key(JsonKey::Type),packet->type);

    return wrappedPack(packet,STATUS_SUCCESS,obj);
}

/*!
     * @brief 为正确信息添加信息头
     * @param[in] packet
     * @return 是否插入成功
     */
QByteArray MsgWrap::wrappedPack(MsgPacket *packet, int status, QJsonObject & data)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Type),packet->msgType);
    obj.insert(JsonKey::key(JsonKey::Command),packet->msgCommand);
    obj.insert(JsonKey::key(JsonKey::Status),status);

    obj.insert(JsonKey::key(JsonKey::Data),data);

    QJsonDocument document;
    document.setObject(obj);

    qDebug()<<document.toJson(QJsonDocument::Indented);

    return document.toJson(QJsonDocument::Compact);
}

QByteArray MsgWrap::wrappedPack(MsgPacket *packet,int status, QJsonArray &data)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Type),packet->msgType);
    obj.insert(JsonKey::key(JsonKey::Command),packet->msgCommand);
    obj.insert(JsonKey::key(JsonKey::Status),status);

    obj.insert(JsonKey::key(JsonKey::Data),data);

    QJsonDocument document;
    document.setObject(obj);

    return document.toJson(QJsonDocument::Compact);
}
