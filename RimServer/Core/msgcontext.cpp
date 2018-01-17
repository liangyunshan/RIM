#include "msgcontext.h"

MsgRequestContext::MsgRequestContext():msgType("type"),msgCommand("cmd"),msgData("data")
{

}

MsgResponseContext::MsgResponseContext():msgType("type"),msgCommand("cmd"),msgData("data"),
    msgStatus("status")
{

}
