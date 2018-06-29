#include "binary716_wrapfromat.h"

Binary716_WrapFromat::Binary716_WrapFromat()
{

}

QByteArray Binary716_WrapFromat::handleMsg(MsgPacket *packet, int result)
{
    if(packet == nullptr)
        return QByteArray();

    switch(packet->msgCommand)
    {
        case MsgCommand::MSG_TEXT_TEXT:
             return handleText((TextRequest *)packet);
        default:
            break;
    }
    return QByteArray();
}

QByteArray Binary716_WrapFromat::handleText(TextRequest *request)
{
    return QByteArray();
}
