#include "msgwrap.h"

#include <QJsonDocument>
#include <QMutexLocker>
#include <QDebug>
#include <QMetaEnum>

#include "Network/netglobal.h"
#include "Util/rlog.h"
#include "Util/rbuffer.h"
#include "jsonkey.h"

MsgWrap::MsgWrap()
{

}

/*!
 * @brief 为普通信息添加信息头
 * @param[in] packet 待发送原始数据包信息
 * @param[in] result 自定义参数，可用于补充头部状态信息
 * @return 添加信息头后的数据信息
 */
QByteArray MsgWrap::handleMsg(MsgPacket *packet, int result)
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

    case MsgCommand::MSG_USER_STATE:
        return handleUserStateChanged((UserStateResponse * )packet);

    case MsgCommand::MSG_RELATION_SEARCH:
        return handleSearchFriendResponse((SearchFriendResponse *)packet);

    case MsgCommand::MSG_RELATION_OPERATE:
        return handleOperateFriendResponse((OperateFriendResponse *)packet);

    case MsgCommand::MSG_RELATION_LIST:
        return handleFriendListResponse((FriendListResponse *)packet,result);

    case MsgCommand::MSG_GROUPING_OPERATE:
        return handleGroupingResponse((GroupingResponse *)packet,result);

    case MsgCommand::MSG_RELATION_GROUPING_FRIEND:
        return handleGroupingFriend((GroupingFriendResponse *)packet,result);

    case MsgCommand::MSG_GROUP_LIST:
        return handleGroupList((ChatGroupListResponse *)packet,result);

    case MsgCommand::MSG_GROUP_CREATE:
        return handleCreateGroup((RegistGroupResponse *)packet,result);

    case MsgCommand::MSG_GROUP_SEARCH:
        return handleSearchGroup((SearchGroupResponse *)packet,result);

    case MsgCommand::MSG_GROUP_OPERATE:
        return handleOpreateGroup((GroupingChatResponse *)packet,result);

    case MsgCommand::MSG_GROUP_COMMAND:
        return handleOpreateCommand((GroupingCommandResponse *)packet,result);

        default:
                break;
    }

    return QByteArray();
}

QByteArray MsgWrap::handleText(TextRequest *request)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::AccountId),request->otherSideId);
    obj.insert(JsonKey::key(JsonKey::TextId),request->textId);
    obj.insert(JsonKey::key(JsonKey::OtherSideId),request->accountId);
    obj.insert(JsonKey::key(JsonKey::OperateType),request->type);
    obj.insert(JsonKey::key(JsonKey::Time),request->timeStamp);
    obj.insert(JsonKey::key(JsonKey::Encryption),request->isEncryption);
    obj.insert(JsonKey::key(JsonKey::Compress),request->isCompress);
    obj.insert(JsonKey::key(JsonKey::Type),request->textType);

    obj.insert(JsonKey::key(JsonKey::Data),request->sendData);

    return wrappedPack(request,STATUS_SUCCESS,obj);
}

QByteArray MsgWrap::handleTextReply(TextReply * response)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::TextId),response->textId);
    obj.insert(JsonKey::key(JsonKey::Type),response->applyType);

    return wrappedPack(response,STATUS_SUCCESS,obj);
}


/*!
 * @brief 处理结果信息
 * @param[in] type 信息类型
 * @param[in] command 命令类型
 * @param[in] replyCode 回复码(客户端可根据信息类型-命令类型-错误码定义具体的错误信息)
 * @param[in] subMsgCommand 二级命令，若一级命令包含了二级命令，则此值有效，其余命令忽略处理
 * @return 序列化数据
 */
QByteArray MsgWrap::handleMsgReply(MsgType type, MsgCommand command, int replyCode, int subMsgCommand)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Type),type);
    obj.insert(JsonKey::key(JsonKey::Command),command);
    obj.insert(JsonKey::key(JsonKey::Status),replyCode);
    obj.insert(JsonKey::key(JsonKey::SubCmd),subMsgCommand);

    QJsonDocument document;
    document.setObject(obj);

    return document.toJson(QJsonDocument::Compact);
}

QByteArray MsgWrap::handleFile(MsgPacket *response)
{
    switch(response->msgCommand)
    {
        case MSG_FILE_CONTROL:
            return handleFileControl((SimpleFileItemRequest *)response);
            break;
        case MSG_FILE_REQUEST:
            return handleFileRequest((FileItemRequest *)response);
            break;
        case MSG_FILE_DATA:
            break;
        default:
            break;
    }
    return QByteArray();
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
    data.insert(JsonKey::key(JsonKey::SystemIcon),packet->baseInfo.isSystemIcon);
    data.insert(JsonKey::key(JsonKey::IconId),packet->baseInfo.iconId);

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
    data.insert(JsonKey::key(JsonKey::SystemIcon),packet->baseInfo.isSystemIcon);
    data.insert(JsonKey::key(JsonKey::IconId),packet->baseInfo.iconId);

    return wrappedPack(packet,UPDATE_USER_SUCCESS,data);
}

QByteArray MsgWrap::handleUserStateChanged(UserStateResponse *packet)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::Status),(int)packet->onStatus);

    return wrappedPack(packet,STATUS_SUCCESS,data);
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
        obj.insert(JsonKey::key(JsonKey::SystemIcon),packet->result.at(i).isSystemIcon);
        obj.insert(JsonKey::key(JsonKey::IconId),packet->result.at(i).iconId);

        data.append(obj);
    }

    return wrappedPack(packet,FIND_FRIEND_FOUND,data);
}

QByteArray MsgWrap::handleOperateFriendResponse(OperateFriendResponse * packet)
{
    QJsonObject obj;

    obj.insert(JsonKey::key(JsonKey::Type),packet->type);
    obj.insert(JsonKey::key(JsonKey::Result),packet->result);
    obj.insert(JsonKey::key(JsonKey::OperateType),packet->stype);
    obj.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    obj.insert(JsonKey::key(JsonKey::ChatId),packet->chatId);
    obj.insert(JsonKey::key(JsonKey::ChatName),packet->chatName);

    QJsonObject requsetInfo;
    requsetInfo.insert(JsonKey::key(JsonKey::AccountId),packet->requestInfo.accountId);
    requsetInfo.insert(JsonKey::key(JsonKey::NickName),packet->requestInfo.nickName);
    requsetInfo.insert(JsonKey::key(JsonKey::SignName),packet->requestInfo.signName);
    requsetInfo.insert(JsonKey::key(JsonKey::SystemIcon),packet->requestInfo.isSystemIcon);
    requsetInfo.insert(JsonKey::key(JsonKey::IconId),packet->requestInfo.iconId);

    obj.insert(JsonKey::key(JsonKey::OperateInfo),requsetInfo);

    return wrappedPack(packet,FRIEND_REQUEST,obj);
}

QByteArray MsgWrap::handleFriendListResponse(FriendListResponse *packet,int result)
{
    QJsonObject obj;

    obj.insert(JsonKey::key(JsonKey::Type),packet->type);
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
            SimpleUserInfo * userInfo = gdata->users.at(j);

            QJsonObject user;
            user.insert(JsonKey::key(JsonKey::AccountId),userInfo->accountId);
            user.insert(JsonKey::key(JsonKey::NickName),userInfo->nickName);
            user.insert(JsonKey::key(JsonKey::SignName),userInfo->signName);
            user.insert(JsonKey::key(JsonKey::SystemIcon),userInfo->isSystemIcon);
            user.insert(JsonKey::key(JsonKey::IconId),userInfo->iconId);
            user.insert(JsonKey::key(JsonKey::Remark),userInfo->remarks);
            user.insert(JsonKey::key(JsonKey::Status),userInfo->status);

            users.append(user);
        }
        groupData.insert(JsonKey::key(JsonKey::Users),users);

        groups.append(groupData);
    }
    obj.insert(JsonKey::key(JsonKey::Groups),groups);

    return wrappedPack(packet,(MsgOperateResponse)result,obj);
}

QByteArray MsgWrap::handleGroupingResponse(GroupingResponse *packet,int result)
{
    QJsonObject obj;
    obj.insert(JsonKey::key(JsonKey::Uuid),packet->uuid);
    obj.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    obj.insert(JsonKey::key(JsonKey::OperateType),packet->gtype);
    obj.insert(JsonKey::key(JsonKey::Type),packet->type);
    obj.insert(JsonKey::key(JsonKey::Index),packet->groupIndex);

    return wrappedPack(packet,(MsgOperateResponse)result,obj);
}

QByteArray MsgWrap::handleGroupingFriend(GroupingFriendResponse *packet,int result)
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

    return wrappedPack(packet,(MsgOperateResponse)result,data);
}

QByteArray MsgWrap::handleGroupList(ChatGroupListResponse *packet, int result)
{
    QJsonObject data;
    data.insert(JsonKey::key(JsonKey::Uuid),packet->uuid);

    QJsonArray groupArray;
    std::for_each(packet->groups.begin(),packet->groups.end(),[&](const RChatGroupData * data){
        QJsonObject chatGroup;
        chatGroup.insert(JsonKey::key(JsonKey::GroupId),data->groupId);
        chatGroup.insert(JsonKey::key(JsonKey::GroupName),data->groupName);
        chatGroup.insert(JsonKey::key(JsonKey::IsDefault),data->isDefault);
        chatGroup.insert(JsonKey::key(JsonKey::Index),data->index);

        QJsonArray simpleChatArray;
        std::for_each(data->chatGroups.begin(),data->chatGroups.end(),[&](const SimpleChatInfo * sdata){
            QJsonObject chatInfo;
            chatInfo.insert(JsonKey::key(JsonKey::Id),sdata->id);
            chatInfo.insert(JsonKey::key(JsonKey::ChatRoomId),sdata->chatRoomId);
            chatInfo.insert(JsonKey::key(JsonKey::ChatId),sdata->chatId);
            chatInfo.insert(JsonKey::key(JsonKey::Remark),sdata->remarks);
            chatInfo.insert(JsonKey::key(JsonKey::NotifyLevel),sdata->messNotifyLevel);
            chatInfo.insert(JsonKey::key(JsonKey::SystemIcon),sdata->isSystemIcon);
            chatInfo.insert(JsonKey::key(JsonKey::IconId),sdata->iconId);
            simpleChatArray.append(chatInfo);
        });

        chatGroup.insert(JsonKey::key(JsonKey::Data),simpleChatArray);

        groupArray.append(chatGroup);
    });

    data.insert(JsonKey::key(JsonKey::Groups),groupArray);

    return wrappedPack(packet,(MsgOperateResponse)result,data);
}

QByteArray MsgWrap::handleCreateGroup(RegistGroupResponse *packet, int result)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::Uuid),packet->userId);
    data.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);

    QJsonObject chatInfoObj;

    chatInfoObj.insert(JsonKey::key(JsonKey::Id),packet->chatInfo.id);
    chatInfoObj.insert(JsonKey::key(JsonKey::ChatRoomId),packet->chatInfo.chatRoomId);
    chatInfoObj.insert(JsonKey::key(JsonKey::ChatId),packet->chatInfo.chatId);
    chatInfoObj.insert(JsonKey::key(JsonKey::Remark),packet->chatInfo.remarks);
    chatInfoObj.insert(JsonKey::key(JsonKey::NotifyLevel),packet->chatInfo.messNotifyLevel);
    chatInfoObj.insert(JsonKey::key(JsonKey::SystemIcon),packet->chatInfo.isSystemIcon);
    chatInfoObj.insert(JsonKey::key(JsonKey::IconId),packet->chatInfo.iconId);

    data.insert(JsonKey::key(JsonKey::Data),chatInfoObj);

    return wrappedPack(packet,(MsgOperateResponse)result,data);
}

QByteArray MsgWrap::handleSearchGroup(SearchGroupResponse *packet, int result)
{
    QJsonArray dataArray;
    std::for_each(packet->result.cbegin(),packet->result.cend(),[&](const ChatBaseInfo & info){
        QJsonObject childObj;

        childObj.insert(JsonKey::key(JsonKey::Uuid),info.uuid);
        childObj.insert(JsonKey::key(JsonKey::ChatId),info.chatId);
        childObj.insert(JsonKey::key(JsonKey::ChatName),info.name);
        childObj.insert(JsonKey::key(JsonKey::Desc),info.desc);
        childObj.insert(JsonKey::key(JsonKey::Label),info.label);
        childObj.insert(JsonKey::key(JsonKey::SearchVisible),info.visible);
        childObj.insert(JsonKey::key(JsonKey::ValidateAble),info.validate);
        childObj.insert(JsonKey::key(JsonKey::Question),info.question);
        childObj.insert(JsonKey::key(JsonKey::Answer),info.answer);
        childObj.insert(JsonKey::key(JsonKey::Users),info.userId);
        childObj.insert(JsonKey::key(JsonKey::SystemIcon),info.isSystemIcon);
        childObj.insert(JsonKey::key(JsonKey::IconId),info.iconId);

        dataArray.append(childObj);
    });

    return wrappedPack(packet,result,dataArray);
}

QByteArray MsgWrap::handleOpreateGroup(GroupingChatResponse *packet, int result)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    data.insert(JsonKey::key(JsonKey::OldGroupId),packet->oldGroupId);

    QJsonObject chatInfoObj;

    chatInfoObj.insert(JsonKey::key(JsonKey::Id),packet->chatInfo.id);
    chatInfoObj.insert(JsonKey::key(JsonKey::ChatRoomId),packet->chatInfo.chatRoomId);
    chatInfoObj.insert(JsonKey::key(JsonKey::ChatId),packet->chatInfo.chatId);
    chatInfoObj.insert(JsonKey::key(JsonKey::Remark),packet->chatInfo.remarks);
    chatInfoObj.insert(JsonKey::key(JsonKey::NotifyLevel),packet->chatInfo.messNotifyLevel);
    chatInfoObj.insert(JsonKey::key(JsonKey::SystemIcon),packet->chatInfo.isSystemIcon);
    chatInfoObj.insert(JsonKey::key(JsonKey::IconId),packet->chatInfo.iconId);

    data.insert(JsonKey::key(JsonKey::Data),chatInfoObj);

    return wrappedPack(packet,(MsgOperateResponse)result,data);
}

QByteArray MsgWrap::handleOpreateCommand(GroupingCommandResponse *packet, int result)
{
    QJsonObject data;

    data.insert(JsonKey::key(JsonKey::RType),packet->respType);
    data.insert(JsonKey::key(JsonKey::Type),packet->type);
    data.insert(JsonKey::key(JsonKey::AccountId),packet->accountId);
    data.insert(JsonKey::key(JsonKey::GroupId),packet->groupId);
    data.insert(JsonKey::key(JsonKey::ChatRoomId),packet->chatRoomId);
    data.insert(JsonKey::key(JsonKey::OperateId),packet->operateId);

    return wrappedPack(packet,(MsgOperateResponse)result,data);
}

#include <QDebug>

QByteArray MsgWrap::handleFileControl(SimpleFileItemRequest *packet)
{
    RBuffer buffer;
    buffer.append((int)packet->msgType);
    buffer.append((int)packet->msgCommand);
    buffer.append((int)STATUS_SUCCESS);
    buffer.append((int)packet->control);
    buffer.append((int)packet->itemType);
    buffer.append((int)packet->itemKind);
    buffer.append(packet->md5);
    buffer.append(packet->fileId);

    return buffer.byteArray();
}

QByteArray MsgWrap::handleFileRequest(FileItemRequest *packet)
{
    RBuffer buffer;
    buffer.append((int)packet->msgType);
    buffer.append((int)packet->msgCommand);
    buffer.append((int)STATUS_SUCCESS);
    buffer.append((int)packet->control);
    buffer.append((int)packet->itemType);
    buffer.append((int)packet->itemKind);
    buffer.append(packet->fileName);
    buffer.append(packet->size);
    buffer.append(packet->fileId);
    buffer.append(packet->md5);
    buffer.append(packet->accountId);
    buffer.append(packet->otherId);

    return buffer.byteArray();
}

//文件数据流
QByteArray MsgWrap::handleFileData(QString fileMd5,size_t currIndex,QByteArray array)
{
    RBuffer rbuffer;
    rbuffer.append((int)MSG_FILE);
    rbuffer.append((int)MSG_FILE_DATA);
    rbuffer.append((int)STATUS_SUCCESS);
    rbuffer.append(fileMd5);
    rbuffer.append(currIndex);
    rbuffer.append(array.data(),array.size());

    return rbuffer.byteArray();
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

//    qDebug()<<document.toJson(QJsonDocument::Indented);

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
