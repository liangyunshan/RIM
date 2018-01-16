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

}


} //namespace Protocol
