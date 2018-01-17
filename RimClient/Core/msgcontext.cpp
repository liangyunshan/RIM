#include "msgcontext.h"

MsgRequestContext::MsgRequestContext():msgType("type"),msgCommand("cmd"),msgData("data")
{

}

MsgResponseContext::MsgResponseContext():msgType("type"),msgCommand("cmd"),msgData("data"),
    msgStatus("status")
{

}

JContext::JContext() :
    magicNum(QLatin1String("magicNum")),
    messType(QLatin1String("messType")),
    commandType(QLatin1String("commandType")),
    timestamp(QLatin1String("timestamp")),
    messLength(QLatin1String("messLength")),
    content(QLatin1String("content"))
{
}
