#include "data716process.h"

#include "rsingleton.h"
#include "messdiapatch.h"
#include "rsingleton.h"
#include "Network/msgwrap/wrapfactory.h"

Data716Process::Data716Process()
{

}

void Data716Process::processTextNoAffirm(const ProtocolPackage &data)
{
    if(data.usOrderNo == O_2048)
    {
        TextReply textReply;
        textReply.textId = QString::number(data.usSerialNo);
        textReply.applyType = APPLY_RECEIPT;

        MessDiapatch::instance()->onRecvTextReply(textReply);
    }
    else
    {
        processText(data);
    }
}

void Data716Process::processTextAffirm(const ProtocolPackage &data)
{
    processText(data);
    ApplyTextStatus(data);
}

void Data716Process::processText(const ProtocolPackage &data)
{
    TextRequest response;

    response.msgCommand = MSG_TEXT_TEXT;
    response.accountId = QString::number(data.wDestAddr);
    response.textId = QString::number(data.usSerialNo);
    response.otherSideId = QString::number(data.wSourceAddr);
    response.type = OperatePerson;
    response.timeStamp = data.usSerialNo;
    response.isEncryption = 0;
    response.isCompress = 0;
    response.textType = TEXT_NORAML;
    response.sendData = QString::fromLocal8Bit( data.data );

    MessDiapatch::instance()->onRecvText(response);
}

void Data716Process::ApplyTextStatus(const ProtocolPackage &data)
{
    //2048回复
    DataPackType request;
    request.extendData.usSerialNo = data.usSerialNo;
    request.extendData.usOrderNo = O_2048;
    request.extendData.type495 = T_DATA_NOAFFIRM;
    request.msgCommand = MSG_TCP_TRANS;
    request.sourceId = QString::number(data.wDestAddr);
    request.destId = QString::number(data.wSourceAddr);
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request);

}

void Data716Process::processTextApply(const ProtocolPackage &data)
{

}
