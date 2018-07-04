#include "rbuffer_msgparse.h"

#include "rsingleton.h"
#include "../msgprocess/binary_dataprocess.h"

RBuffer_MsgParse::RBuffer_MsgParse()
{

}

#ifdef __LOCAL_CONTACT__
void RBuffer_MsgParse::processData(const RecvUnit &unit)
{
    RBuffer buffer(unit.data);
    int type;
    if(!buffer.read(type))
        return;
    if((int)type == MSG_FILE)
    {
        int msgCommand;
        if(!buffer.read(msgCommand))
            return;
        handleFileMsg((MsgCommand)msgCommand,buffer);
    }
}

#else
void RBuffer_MsgParse::processData(const ProtocolPackage &recvData)
{
    RBuffer buffer(recvData.data);
    int type;
    if(!buffer.read(type))
        return;
    if((int)type == MSG_FILE)
    {
        int msgCommand;
        if(!buffer.read(msgCommand))
            return;
        handleFileMsg((MsgCommand)msgCommand,buffer);
    }
}
#endif

void RBuffer_MsgParse::handleFileMsg(MsgCommand commandType, RBuffer &obj)
{
    switch(commandType)
    {
        case MSG_FILE_CONTROL:
            RSingleton<Binary_DataProcess>::instance()->proFileControl(obj);
            break;

        case MSG_FILE_REQUEST:
            RSingleton<Binary_DataProcess>::instance()->proFileRequest(obj);
            break;

        case MSG_FILE_DATA:
            RSingleton<Binary_DataProcess>::instance()->proFileData(obj);
            break;

        default:
            break;
    }
}
