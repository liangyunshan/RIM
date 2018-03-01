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
        case MsgCommand::MSG_USER_UPDATE_INFO:
                handleUpdateBaseInfoRequest((UpdateBaseInfoRequest *)packet);
                break;
        case MsgCommand::MSG_USER_STATE:
                handleUserStateRequest((UserStateRequest*)packet);
                break;
        case MsgCommand::MSG_RELATION_SEARCH:
                handleSearchFriendRequest((SearchFriendRequest *)packet);
                break;
        case MsgCommand::MSG_REALTION_ADD:
                handleAddFriendRequest((AddFriendRequest *)packet);
                break;
        case MsgCommand::MSG_RELATION_OPERATE:
                handleOperateFriendRequest((OperateFriendRequest *)packet);
                break;
        case MsgCommand::MSG_RELATION_LIST:
                handleFriendListRequest((FriendListRequest *)packet);
                break;
        case MsgCommand::MSG_GROUPING_OPERATE:
                handleGroupingOperateRequest((GroupingRequest *)packet);
                break;
        case MsgCommand::MSG_RELATION_GROUPING_FRIEND:
                handleGroupingFriendRequest((GroupingFriendRequest *)packet);
                break;
        default:
                break;
    }
}

void MsgWrap::hanleText(TextRequest *packet)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::TextId),packet->textId);
    data.insert(JsonKey::key(JsonKey::OtherSideId),packet->otherSideId);
    data.insert(JsonKey::key(JsonKey::SearchType),packet->type);
    data.insert(JsonKey::key(JsonKey::Time),packet->timeStamp);
    data.insert(JsonKey::key(JsonKey::Encryption),packet->isEncryption);
    data.insert(JsonKey::key(JsonKey::Compress),packet->isCompress);
    data.insert(JsonKey::key(JsonKey::Type),packet->textType);
    //TODO 数据压缩
    data.insert(JsonKey::key(JsonKey::Data),packet->sendData);

    wrappedPack(packet,data);
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

/*!
 * @brief 处理用户更新个人基本信息
 * @param[in] packet 用户基本信息请求
 * @return 无
 */
void MsgWrap::handleUpdateBaseInfoRequest(UpdateBaseInfoRequest * packet)
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

    wrappedPack(packet,data);
}

/*!
 * @brief 处理用户状态变更
 * @param[in] request 状态变更请求
 * @return 无
 */
void MsgWrap::handleUserStateRequest(UserStateRequest * request)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AccountId),request->accountId);
    data.insert(JsonKey::key(JsonKey::Status),(int)request->onStatus);

    wrappedPack(request,data);
}

/*!
 * @brief 处理查找好友信息
 * @param[in] packet 查询好友请求
 * @return 无
 */
void MsgWrap::handleSearchFriendRequest(SearchFriendRequest * packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::SearchType),packet->stype);
    data.insert(JsonKey::key(JsonKey::SearchContent),packet->accountOrNickName);

    wrappedPack(packet,data);
}

//处理添加好友
void MsgWrap::handleAddFriendRequest(AddFriendRequest * packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AddType),packet->stype);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::OperateId),packet->operateId);

    wrappedPack(packet,data);
}

//处理用户操作请求
void MsgWrap::handleOperateFriendRequest(OperateFriendRequest * packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::OperateId),packet->operateId);
    data.insert(JsonKey::key(JsonKey::Result),packet->result);
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::SearchType),packet->stype);

    wrappedPack(packet,data);
}

//处理用户信息列表请求
void MsgWrap::handleFriendListRequest(FriendListRequest *packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    wrappedPack(packet,data);
}

//处理用户分组操作
void MsgWrap::handleGroupingOperateRequest(GroupingRequest *packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::Uuid),packet->uuid);
    data.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    data.insert(JsonKey::key(JsonKey::GroupType),packet->gtype);
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::GroupName),packet->groupName);

    wrappedPack(packet,data);
}

void MsgWrap::handleGroupingFriendRequest(GroupingFriendRequest * packet)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    data.insert(JsonKey::key(JsonKey::OldGroupId),packet->oldGroupId);
    data.insert(JsonKey::key(JsonKey::SearchType),packet->stype);

    QJsonObject user;
    user.insert(JsonKey::key(JsonKey::AccountId),packet->user.accountId);
    user.insert(JsonKey::key(JsonKey::NickName),packet->user.nickName);
    user.insert(JsonKey::key(JsonKey::SignName),packet->user.signName);
    user.insert(JsonKey::key(JsonKey::Face),packet->user.face);
    user.insert(JsonKey::key(JsonKey::FaceId),packet->user.customImgId);
    user.insert(JsonKey::key(JsonKey::Remark),packet->user.remarks);

    data.insert(JsonKey::key(JsonKey::Users),user);

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

    if(packet->isAutoDelete)
    {
        delete packet;
    }

    G_SendMutex.lock();
    G_SendBuff.enqueue(array);
    G_SendMutex.unlock();

    G_SendWaitCondition.wakeOne();

//    RLOG_INFO("Send msg:%s",array.data());
}
