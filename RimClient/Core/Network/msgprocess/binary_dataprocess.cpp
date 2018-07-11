#include "binary_dataprocess.h"

#include "util/rsingleton.h"
#include "file/filedesc.h"
#include "file/filemanager.h"
#include "messdiapatch.h"

#include "../../protocol/protocoldata.h"
using namespace ProtocolType;

#define FILE_MAX_PACK_SIZE 900      //文件每包最大的长度

Binary_DataProcess::Binary_DataProcess()
{

}

/*!
 * @attention 注意RBuffer中数据的开始指针已经不是指向0的位置
 */
void Binary_DataProcess::proFileControl(RBuffer &data)
{
    SimpleFileItemRequest simpleControl;

    int status;
    if(!data.read(status))
        return;
    if((MsgOperateResponse)status == STATUS_SUCCESS)
    {
        int control;
        if(!data.read(control))
            return;
        simpleControl.control = (FileTransferControl)control;

        int itemType;
        if(!data.read(itemType))
            return;
        simpleControl.itemType = static_cast<FileItemType>(itemType);

        int itemKind;
        if(!data.read(itemKind))
            return;
        simpleControl.itemKind = static_cast<FileItemKind>(itemKind);

        if(!data.read(simpleControl.md5))
            return;

        if(!data.read(simpleControl.fileId))
            return;

        MessDiapatch::instance()->onRecvFileControl(simpleControl);
    }
}

/*!
 * @attention 注意RBuffer中数据的开始指针已经不是指向0的位置
 */
void Binary_DataProcess::proFileRequest(RBuffer & data)
{
    FileItemRequest itemRequest;

    int status;
    if(!data.read(status))
        return;
    if((MsgOperateResponse)status == STATUS_SUCCESS)
    {
        int control;
        if(!data.read(control))
            return;
        itemRequest.control = (FileTransferControl)control;

        int itemType;
        if(!data.read(itemType))
            return;
        itemRequest.itemType = static_cast<FileItemType>(itemType);

        int itemKind;
        if(!data.read(itemKind))
            return;
        itemRequest.itemKind = static_cast<FileItemKind>(itemKind);

        if(!data.read(itemRequest.fileName))
            return;

        if(!data.read(itemRequest.size))
            return;

        if(!data.read(itemRequest.fileId))
            return;

        if(!data.read(itemRequest.md5))
            return;

        if(!data.read(itemRequest.accountId))
            return;

        if(!data.read(itemRequest.otherId))
            return;

        MessDiapatch::instance()->onRecvFileRequest(itemRequest);
    }
}

void Binary_DataProcess::proFileData(RBuffer &data)
{
    FileDataRequest request;

    int control = 0;

    if(!data.read(control))
        return;
    request.control = static_cast<FileTransferControl>(control);

    if(!data.read(request.fileId))
        return;

    if(!data.read(request.index))
        return;

    const char * recvData = NULL;
    size_t dataLen = 0;
    if(!data.read(&recvData,BUFFER_DEFAULT_SIZE,dataLen))
        return;
    request.array.append(recvData,dataLen);

    FileDesc * fileDesc = RSingleton<FileManager>::instance()->getFile(request.fileId);
    if(fileDesc)
    {
        if(fileDesc->isNull() && !fileDesc->create())
        {
            return;
        }

//        if(fileDesc->state() == FILE_TRANING || fileDesc->state() == FILE_PAUSE)
        if(fileDesc->seek(request.index * FILE_MAX_PACK_SIZE) && fileDesc->write(request.array) > 0)
        {
            if(fileDesc->flush() && fileDesc->isRecvOver())
            {
                fileDesc->close();

                MessDiapatch::instance()->onRecvFileData(fileDesc->fileId,fileDesc->fileName);
            }
        }
    }
}
