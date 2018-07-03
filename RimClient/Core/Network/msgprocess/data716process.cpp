#include "data716process.h"

#include "rsingleton.h"
#include "messdiapatch.h"

Data716Process::Data716Process()
{

}

void Data716Process::processTextNoAffirm(ProtocolPackage &data)
{
    processText(data);
}

void Data716Process::processTextAffirm(ProtocolPackage &data)
{
    processText(data);
}

void Data716Process::processText(ProtocolPackage &data)
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

void Data716Process::ApplyTextStatus()
{

}

void Data716Process::processTextApply(ProtocolPackage &data)
{

}
