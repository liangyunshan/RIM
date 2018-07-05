#include "binary_wrapformat.h"

#include "Util/rbuffer.h"

Binary_WrapFormat::Binary_WrapFormat():
    WrapFormat()
{

}

QByteArray Binary_WrapFormat::handleMsg(MsgPacket * packet,int result)
{
    switch(packet->msgCommand)
    {
        case MSG_FILE_CONTROL:
            return handleFileControl((SimpleFileItemRequest *)packet);
            break;
        case MSG_FILE_REQUEST:
            return handleFileRequest((FileItemRequest *)packet);
            break;
        case MSG_FILE_DATA:
            return handleFileData((FileDataTrunkRequest *)packet);
        default:
            break;
    }
    return QByteArray();
}

QByteArray Binary_WrapFormat::handleMsgReply(MsgType type, MsgCommand command, int replyCode, int subMsgCommand)
{
    return QByteArray();
}

QByteArray Binary_WrapFormat::handleFileControl(SimpleFileItemRequest *packet)
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

QByteArray Binary_WrapFormat::handleFileRequest(FileItemRequest *packet)
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
QByteArray Binary_WrapFormat::handleFileData(FileDataTrunkRequest * packet)
{
    RBuffer rbuffer;
    rbuffer.append((int)packet->msgType);
    rbuffer.append((int)packet->msgCommand);
    rbuffer.append((int)STATUS_SUCCESS);
    rbuffer.append(packet->md5);
    rbuffer.append(packet->currIndex);
    rbuffer.append(packet->data.data(),packet->data.size());

    return rbuffer.byteArray();
}

