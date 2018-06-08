#include "binary_wrapformat.h"

#include "Util/rbuffer.h"

Binary_WrapFormat::Binary_WrapFormat():WrapFormat()
{

}

Binary_WrapFormat::~Binary_WrapFormat()
{

}

void Binary_WrapFormat::handleMsg(MsgPacket *packet, QByteArray &result)
{
    if(packet == nullptr)
        return;

    switch(packet->msgCommand){
        case MsgCommand::MSG_FILE_CONTROL:
                handelFileControl((SimpleFileItemRequest *)packet,result);
                break;
        case MsgCommand::MSG_FILE_REQUEST:
                handleFileRequest((FileItemRequest *)packet,result);
                break;
        case MsgCommand::MSG_FILE_DATA:
                handleFileData((FileDataRequest *)packet,result);
                break;
        default:
            break;
    }
}

void Binary_WrapFormat::handelFileControl(SimpleFileItemRequest *request, QByteArray &result)
{
    RBuffer rbuffer;
    rbuffer.append((int)request->msgType);
    rbuffer.append((int)request->msgCommand);
    rbuffer.append((int)request->control);
    rbuffer.append((int)request->itemType);
    rbuffer.append((int)request->itemKind);
    rbuffer.append(request->md5);
    rbuffer.append(request->fileId);

    if(request->isAutoDelete)
        delete request;

    result = rbuffer.byteArray();
}

void Binary_WrapFormat::handleFileRequest(FileItemRequest *fileRequest, QByteArray &result)
{
    RBuffer rbuffer;
    fileRequest->control = T_ABLE_SEND;
    rbuffer.append((int)fileRequest->msgType);
    rbuffer.append((int)fileRequest->msgCommand);
    rbuffer.append((int)fileRequest->control);
    rbuffer.append((int)fileRequest->itemType);
    rbuffer.append((int)fileRequest->itemKind);
    rbuffer.append(fileRequest->fileName);
    rbuffer.append(fileRequest->size);
    rbuffer.append(fileRequest->fileId);
    rbuffer.append(fileRequest->md5);
    rbuffer.append(fileRequest->accountId);
    rbuffer.append(fileRequest->otherId);

    if(fileRequest->isAutoDelete)
        delete fileRequest;

    result = rbuffer.byteArray();
}

//文件数据流
void Binary_WrapFormat::handleFileData(FileDataRequest * dataRequest,QByteArray &result)
{
    RBuffer rbuffer;
    rbuffer.append((int)MSG_FILE);
    rbuffer.append((int)MSG_FILE_DATA);
    rbuffer.append(dataRequest->fileId);
    rbuffer.append(dataRequest->index);
    rbuffer.append(dataRequest->array.data(),dataRequest->array.size());

    if(dataRequest->isAutoDelete)
        delete dataRequest;
    result = rbuffer.byteArray();
}

