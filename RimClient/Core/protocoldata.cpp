#include "protocoldata.h"

namespace ProtocolType {

MsgPacket::MsgPacket()
{
    msgType = MSG_TYPE_INVALID;
    msgCommand = MSG_Command_INVALID;

    isAutoDelete = true;
}

MsgPacket::~MsgPacket()
{

}

LoginRequest::LoginRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_USER_LOGIN;
}

LoginResponse::LoginResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_USER_LOGIN;
}

RegistRequest::RegistRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_USER_REGISTER;
}

RegistResponse::RegistResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_USER_REGISTER;
}

UpdateBaseInfoRequest::UpdateBaseInfoRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_USER_UPDATE_INFO;
}

UpdateBaseInfoResponse::UpdateBaseInfoResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_USER_UPDATE_INFO;
}

SearchFriendRequest::SearchFriendRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_RELATION_SEARCH;
}

SearchFriendResponse::SearchFriendResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_RELATION_SEARCH;
}

AddFriendRequest::AddFriendRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_REALTION_ADD;
}

AddFriendResponse::AddFriendResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_REALTION_ADD;
}

OperateFriendRequest::OperateFriendRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_RELATION_OPERATE;
}

OperateFriendResponse::OperateFriendResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_RELATION_OPERATE;
}

FriendListRequest::FriendListRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_RELATION_LIST;
}

FriendListResponse::FriendListResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_RELATION_LIST;
}

FriendListResponse::~FriendListResponse()
{
    QList<RGroupData *>::iterator iter = groups.begin();
    while((iter != groups.end()))
    {
        delete (*iter);
        iter = groups.erase(iter);
    }
}

GroupingRequest::GroupingRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_GROUPING_OPERATE;
}

GroupingResponse::GroupingResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_GROUPING_OPERATE;
}

TextRequest::TextRequest()
{
    msgType = MSG_TEXT;
    msgCommand = MSG_TEXT_TEXT;

    isAutoDelete = false;
}

GroupingFriendRequest::GroupingFriendRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_RELATION_GROUPING_FRIEND;
}

GroupingFriendResponse::GroupingFriendResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_RELATION_GROUPING_FRIEND;
}

UserStateRequest::UserStateRequest()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_USER_STATE;
}

UserStateResponse::UserStateResponse()
{
    msgType = MSG_CONTROL;
    msgCommand = MSG_USER_STATE;
}

RGroupData::~RGroupData()
{
    QList<SimpleUserInfo *>::iterator iter = users.begin();
    while(iter != users.end())
    {
        delete (*iter);
        iter = users.erase(iter);
    }
}

TextReply::TextReply()
{
    msgType = MSG_TEXT;
    msgCommand = MSG_TEXT_APPLY;
}

FileItemRequest::FileItemRequest()
{
    msgType = MSG_FILE;
}


} //namespace Protocol
