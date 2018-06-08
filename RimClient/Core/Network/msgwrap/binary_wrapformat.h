#ifndef BINARY_WRAPFORMAT_H
#define BINARY_WRAPFORMAT_H

#include "wrapformat.h"

using namespace ProtocolType;

class Binary_WrapFormat : public WrapFormat
{
public:
    explicit Binary_WrapFormat();
    ~Binary_WrapFormat();

    void handleMsg(MsgPacket * packet,QByteArray & result);

private:
    void handelFileControl(SimpleFileItemRequest * request, QByteArray &result);
    void handleFileRequest(FileItemRequest * fileRequest, QByteArray &result);
    void handleFileData(FileDataRequest * dataRequest,QByteArray &result);
};

#endif // BINARY_MSGWRAP_H
