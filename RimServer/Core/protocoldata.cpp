#include "protocoldata.h"

namespace ProtocolType {

MsgPacket::MsgPacket()
{
    msgType = MSG_TYPE_INVALID;
    msgCommand = MSG_Command_INVALID;
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


} //namespace Protocol
