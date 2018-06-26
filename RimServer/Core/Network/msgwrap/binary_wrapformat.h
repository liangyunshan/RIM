#ifndef BINARY_WRAPFORMAT_H
#define BINARY_WRAPFORMAT_H

#include "wrapformat.h"

class Binary_WrapFormat : public WrapFormat
{
public:
    explicit Binary_WrapFormat();

    QByteArray handleMsg(MsgPacket * packet,int result = 0);

    QByteArray handleMsgReply(MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1);

private:
    QByteArray handleFileControl(SimpleFileItemRequest * packet);
    QByteArray handleFileRequest(FileItemRequest * packet);
    QByteArray handleFileData(FileDataTrunkRequest * packet);
};

#endif // BINARY_WRAPFORMAT_H
