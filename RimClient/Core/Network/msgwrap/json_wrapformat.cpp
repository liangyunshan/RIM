#include "json_wrapformat.h"

#include <QJsonDocument>

#include "jsonkey.h"
#include "../../protocol/protocoldata.h"

Json_WrapFormat::Json_WrapFormat():WrapFormat()
{

}

void Json_WrapFormat::handleMsg(MsgPacket *packet, QByteArray &result)
{
    if(packet == nullptr)
        return;

    switch(packet->msgCommand){
        case MsgCommand::MSG_USER_REGISTER:
                handleRegistRequest((RegistRequest *)packet,result);
                break;
        case MsgCommand::MSG_USER_LOGIN:
                handleLoginRequest((LoginRequest *)packet,result);
                break;
        case MsgCommand::MSG_USER_UPDATE_INFO:
                handleUpdateBaseInfoRequest((UpdateBaseInfoRequest *)packet,result);
                break;
        case MsgCommand::MSG_USER_STATE:
                handleUserStateRequest((UserStateRequest*)packet,result);
                break;
        case MsgCommand::MSG_RELATION_SEARCH:
                handleSearchFriendRequest((SearchFriendRequest *)packet,result);
                break;
        case MsgCommand::MSG_REALTION_ADD:
                handleAddFriendRequest((AddFriendRequest *)packet,result);
                break;
        case MsgCommand::MSG_RELATION_OPERATE:
                handleOperateFriendRequest((OperateFriendRequest *)packet,result);
                break;
        case MsgCommand::MSG_RELATION_LIST:
                handleFriendListRequest((FriendListRequest *)packet,result);
                break;
        case MsgCommand::MSG_GROUPING_OPERATE:
                handleGroupingOperateRequest((GroupingRequest *)packet,result);
                break;
        case MsgCommand::MSG_RELATION_GROUPING_FRIEND:
                handleGroupingFriendRequest((GroupingFriendRequest *)packet,result);
                break;
        case MSG_GROUP_LIST:
                handleChatGroupListRequest((ChatGroupListRequest *)packet,result);
                break;
        case MSG_GROUP_CREATE:
                handleRegistGroupRequest((RegistGroupRequest *)packet,result);
                break;
        case MSG_GROUP_COMMAND:
                handleGroupCommandRequest((GroupingCommandRequest *)packet,result);
                break;
        case MSG_USER_HISTORY_MSG:
                handleHistoryMsgRequest((HistoryMessRequest *)packet,result);
                break;

        case MsgCommand::MSG_TEXT_TEXT:
                handleTextRequest((TextRequest *)packet,result);
                break;
        default:
            break;
    }
}

//处理用户登陆
void Json_WrapFormat::handleLoginRequest(LoginRequest *packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::LoginType),packet->loginType);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::Pass),packet->password);
    data.insert(JsonKey::key(JsonKey::Status),packet->status);

    wrappedPack(packet,data,result);
}

//处理用户注册
void Json_WrapFormat::handleRegistRequest(RegistRequest *packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::NickName),packet->nickName);
    data.insert(JsonKey::key(JsonKey::Pass),packet->password);

    wrappedPack(packet,data,result);
}

/*!
 * @brief 处理用户更新个人基本信息
 * @param[in] packet 用户基本信息请求
 * @return 无
 */
void Json_WrapFormat::handleUpdateBaseInfoRequest(UpdateBaseInfoRequest * packet, QByteArray &result)
{
    QJsonObject data;
    if(packet->requestType == UPDATE_USER_DETAIL)
    {
        data.insert(JsonKey::key(JsonKey::AccountId),packet->baseInfo.accountId);
        data.insert(JsonKey::key(JsonKey::NickName),packet->baseInfo.nickName);
        data.insert(JsonKey::key(JsonKey::SignName),packet->baseInfo.signName);
        data.insert(JsonKey::key(JsonKey::Sexual),packet->baseInfo.sexual);
        data.insert(JsonKey::key(JsonKey::Birth),packet->baseInfo.birthday);
        data.insert(JsonKey::key(JsonKey::Address),packet->baseInfo.address);
        data.insert(JsonKey::key(JsonKey::Email),packet->baseInfo.email);
        data.insert(JsonKey::key(JsonKey::Phone),packet->baseInfo.phoneNumber);
        data.insert(JsonKey::key(JsonKey::Remark),packet->baseInfo.remark);
        data.insert(JsonKey::key(JsonKey::SystemIcon),packet->baseInfo.isSystemIcon);
        data.insert(JsonKey::key(JsonKey::IconId),packet->baseInfo.iconId);
        data.insert(JsonKey::key(JsonKey::Type),packet->requestType);
    }
    else
    {
        data.insert(JsonKey::key(JsonKey::AccountId),packet->baseInfo.accountId);
        data.insert(JsonKey::key(JsonKey::Type),packet->requestType);
    }


    wrappedPack(packet,data,result);
}

/*!
 * @brief 处理用户状态变更
 * @param[in] request 状态变更请求
 * @return 无
 */
void Json_WrapFormat::handleUserStateRequest(UserStateRequest * request, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AccountId),request->accountId);
    data.insert(JsonKey::key(JsonKey::Status),(int)request->onStatus);

    wrappedPack(request,data,result);
}

/*!
 * @brief 处理查找好友信息
 * @param[in] packet 查询好友请求
 * @return 无
 */
void Json_WrapFormat::handleSearchFriendRequest(SearchFriendRequest * packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::OperateType),packet->stype);
    data.insert(JsonKey::key(JsonKey::SearchContent),packet->accountOrNickName);

    wrappedPack(packet,data,result);
}

//处理添加好友
void Json_WrapFormat::handleAddFriendRequest(AddFriendRequest * packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AddType),packet->stype);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::OperateId),packet->operateId);

    wrappedPack(packet,data,result);
}

//处理用户操作请求
void Json_WrapFormat::handleOperateFriendRequest(OperateFriendRequest * packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::ChatId),packet->chatId);
    data.insert(JsonKey::key(JsonKey::ChatName),packet->chatName);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::OperateId),packet->operateId);
    data.insert(JsonKey::key(JsonKey::Result),packet->result);
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::OperateType),packet->stype);

    wrappedPack(packet,data,result);
}

//处理用户信息列表请求
void Json_WrapFormat::handleFriendListRequest(FriendListRequest *packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    wrappedPack(packet,data,result);
}

//处理用户分组操作
void Json_WrapFormat::handleGroupingOperateRequest(GroupingRequest *packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::Uuid),packet->uuid);
    data.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    data.insert(JsonKey::key(JsonKey::OperateType),packet->gtype);
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::GroupName),packet->groupName);
    data.insert(JsonKey::key(JsonKey::Index),packet->groupIndex);

    wrappedPack(packet,data,result);
}

void Json_WrapFormat::handleGroupingFriendRequest(GroupingFriendRequest * packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    data.insert(JsonKey::key(JsonKey::OldGroupId),packet->oldGroupId);
    data.insert(JsonKey::key(JsonKey::OperateType),packet->stype);

    QJsonObject user;
    user.insert(JsonKey::key(JsonKey::AccountId),packet->user.accountId);
    user.insert(JsonKey::key(JsonKey::NickName),packet->user.nickName);
    user.insert(JsonKey::key(JsonKey::SignName),packet->user.signName);
    user.insert(JsonKey::key(JsonKey::SystemIcon),packet->user.isSystemIcon);
    user.insert(JsonKey::key(JsonKey::IconId),packet->user.iconId);
    user.insert(JsonKey::key(JsonKey::Remark),packet->user.remarks);
    user.insert(JsonKey::key(JsonKey::Status),packet->user.status);

    data.insert(JsonKey::key(JsonKey::Users),user);

    wrappedPack(packet,data,result);
}

void Json_WrapFormat::handleChatGroupListRequest(ChatGroupListRequest *packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::Uuid),packet->uuid);
    wrappedPack(packet,data,result);
}

void Json_WrapFormat::handleRegistGroupRequest(RegistGroupRequest *packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::GroupName),packet->groupName);
    data.insert(JsonKey::key(JsonKey::Desc),packet->groupDesc);
    data.insert(JsonKey::key(JsonKey::Label),packet->groupLabel);
    data.insert(JsonKey::key(JsonKey::SearchVisible),packet->searchVisible);
    data.insert(JsonKey::key(JsonKey::ValidateAble),packet->validateAble);
    data.insert(JsonKey::key(JsonKey::Question),packet->validateQuestion);
    data.insert(JsonKey::key(JsonKey::Answer),packet->validateAnaswer);
    data.insert(JsonKey::key(JsonKey::Uuid),packet->userId);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);

    wrappedPack(packet,data,result);
}

void Json_WrapFormat::handleGroupCommandRequest(GroupingCommandRequest *packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    data.insert(JsonKey::key(JsonKey::ChatRoomId),packet->chatRoomId);
    data.insert(JsonKey::key(JsonKey::OperateId),packet->operateId);

    wrappedPack(packet,data,result);
}

void Json_WrapFormat::handleHistoryMsgRequest(HistoryMessRequest *packet, QByteArray &result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);

    wrappedPack(packet,data,result);
}

/*!
 * @brief 包装文字信息请求
 * @param[in] packet 文件请求内容
 * @param[in/out] result 结果信息
 */
void Json_WrapFormat::handleTextRequest(TextRequest *packet,QByteArray& result)
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

void Json_WrapFormat::wrappedPack(MsgPacket *packet,QJsonObject & data,QByteArray& result)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Type),packet->msgType);
    obj.insert(JsonKey::key(JsonKey::Command),packet->msgCommand);

    obj.insert(JsonKey::key(JsonKey::Data),data);

    QJsonDocument document;
    document.setObject(obj);
    result = document.toJson(QJsonDocument::Compact);

    if(packet->isAutoDelete){

    }
//        delete packet;
}
