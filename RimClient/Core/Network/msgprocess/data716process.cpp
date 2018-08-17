#include "data716process.h"

#ifdef __LOCAL_CONTACT__

#include <QDebug>
#include <qmath.h>
#include <QDir>

#include "util/rsingleton.h"
#include "messdiapatch.h"
#include "util/rsingleton.h"
#include "global.h"
#include "file/filedesc.h"
#include "file/filemanager.h"
#include "Network/msgwrap/wrapfactory.h"
#include "Network/network_global.h"
#include "Util/rutil.h"
#include "Util/scaleswitcher.h"
#include "../../user/user.h"

using namespace QDB495;
using namespace QDB21;
using namespace QDB2051;

/*!
 * @brief 查询指定节点的通信配置方式
 * @param[in] nodeId 十进制待查询的节点号
 * @param[in] success 是否查询成功的标识
 */
OuterNetConfig QueryNodeConfig(unsigned short nodeId,bool & success )
{
    success = false;

    if(Global::G_ParaSettings){
        auto index = std::find_if(Global::G_ParaSettings->outerNetConfig.begin(),Global::G_ParaSettings->outerNetConfig.end(),[&nodeId](const OuterNetConfig & conf){
            return nodeId == ScaleSwitcher::fromHexToDec(const_cast<QString&>(conf.nodeId));
        });

        if(index != Global::G_ParaSettings->outerNetConfig.end()){
            success = true;
            return (*index);
        }
    }
    return OuterNetConfig();
}

Data716Process::Data716Process()
{

}

void Data716Process::processTextNoAffirm(const ProtocolPackage &data)
{
    if(data.usOrderNo == O_2048)
    {
        TextReply textReply;
        textReply.wSourceAddr = ScaleSwitcher::fromDecToHex(data.wSourceAddr);
        textReply.wDestAddr = ScaleSwitcher::fromDecToHex(data.wDestAddr);
        textReply.textId = QString::number(data.usSerialNo);
        textReply.applyType = APPLY_RECEIPT;
        MessDiapatch::instance()->onRecvTextReply(textReply);
    }
    else
    {
        processText(data);
    }
}

/*!
 * @brief 处理需要回执的信息
 * @param[in] data 待处理的原始数据
 */
void Data716Process::processTextAffirm(const ProtocolPackage &data)
{
    processText(data);
    applyTextStatus(data);
}

void Data716Process::processText(const ProtocolPackage &data)
{
    TextRequest response;

    response.msgCommand = MSG_TEXT_TEXT;
    response.accountId = ScaleSwitcher::fromDecToHex(data.wDestAddr);
    response.otherSideId = ScaleSwitcher::fromDecToHex(data.wSourceAddr);
    response.textId = QString::number(data.usSerialNo);
    response.type = OperatePerson;

    QDate t_date = QDate::fromString(QString::number(data.cDate),QString("yyyyMMdd"));
    QTime t_time = QTime::fromString(QString::number(data.cTime),QString("hhmmss"));
    QDateTime t_curMsgTime = QDateTime(t_date,t_time);
    qint64 t_Time = t_curMsgTime.toMSecsSinceEpoch();

    response.timeStamp = t_Time;
    response.isEncryption = 0;
    response.isCompress = 0;
    response.textType = TEXT_NORAML;
    response.sendData = QString::fromLocal8Bit( data.data );

    MessDiapatch::instance()->onRecvText(response);
}

/*!
 * @brief 发送回执信息
 * @note 接收信息后，主动发送回执信息。不同的协议对应的回执类型也是不同的。 \n
 * @param[in] data 接收的原始数据信息
 */
void Data716Process::applyTextStatus(const ProtocolPackage &data)
{
    bool success = false;
    OuterNetConfig conf = QueryNodeConfig(data.wSourceAddr,success);
    if(success){
        DataPackType request;
        request.extendData.usSerialNo = data.usSerialNo;
        switch(conf.communicationMethod){
            case C_TongKong:
                {
                    request.extendData.usOrderNo = O_2048;
                }
                break;
            default:
                    break;
        }

        request.extendData.type495 = T_DATA_NOAFFIRM;
        request.msgCommand = MSG_TCP_TRANS;
        request.sourceId = ScaleSwitcher::fromDecToHex(data.wDestAddr);
        request.destId = ScaleSwitcher::fromDecToHex(data.wSourceAddr);
        RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,conf.communicationMethod,conf.messageFormat);
    }
}

void Data716Process::processTextApply(ProtocolPackage &data)
{

}

/*!
 * @brief 客户端处理接收的文件块
 * @note 客户端在判断是否为同一文件时，需要根据发送方的节点号+流水号+文件名 \n
 * @param[in] data 原始数据
 */
void Data716Process::processFileData(const ProtocolPackage &data)
{
    QString fUUID = QString("%1_%2_%3")
            .arg(QString::number(data.wSourceAddr,16))
            .arg(QString::number(data.wDestAddr,16))
            .arg(data.usSerialNo);
    std::shared_ptr<FileDesc> fileDesc = RSingleton<FileManager>::instance()->get716File(fUUID);
    if(fileDesc.get() == nullptr)
    {
        fileDesc = std::make_shared<FileDesc>();

        fileDesc->setFilePath(G_User->getC2CFilePath());

        fileDesc->md5 = RUtil::UUID();
        fileDesc->itemKind = (int)data.cFileType;
        fileDesc->bPackType = data.bPackType;
        fileDesc->fileName = QString::fromLocal8Bit(data.cFilename);
        fileDesc->accountId = QString::number(data.wSourceAddr,16);
        fileDesc->otherId = QString::number(data.wDestAddr,16);
        fileDesc->usOrderNo = data.usOrderNo;
        fileDesc->usSerialNo = data.usSerialNo;
        fileDesc->fileId = fUUID;

        int protocolDataLen = QDB21_Head_Length + QDB2051_Head_Length;
        fileDesc->fileHeadLen = protocolDataLen + data.cFilename.size();
        fileDesc->size = data.dwPackAllLen - fileDesc->fileHeadLen ;
        fileDesc->writeLen = 0;

        if(!fileDesc->create())
            return;

        if(!RSingleton<FileManager>::instance()->addFile(fileDesc))
            return;
    }

    if(fileDesc->seek(data.wOffset > 0 ? (data.wOffset * MAX_PACKET - fileDesc->fileHeadLen) : 0) && fileDesc->write(data.data) > 0)
    {
        //TODO 对接收文件进行处理
        FileTransProgress progress;
        progress.transType = TRANS_RECV;
        progress.serialNo = QString::number(fileDesc->usSerialNo);
        progress.srcNodeId = fileDesc->accountId;
        progress.destNodeId = fileDesc->otherId;
        progress.fileName = fileDesc->fileName;
        progress.fileFullPath = G_User->getC2CFilePath() + QDir::separator() + fileDesc->fileName;
        progress.readySendBytes = fileDesc->writeLen;
        progress.totleBytes = fileDesc->size;

        if(fileDesc->flush() && fileDesc->isRecvOver())
        {
            fileDesc->close();
            progress.transStatus = TransSuccess;
            RSingleton<FileManager>::instance()->removeFile(fileDesc->fileId);

            MessDiapatch::instance()->onFileTransStatusChanged(progress);
        }
        else
        {
            if(data.wOffset == 0)
            {
                progress.transStatus = TransStart;
            }
            else
            {
                progress.transStatus = TransProcess;
            }
            if(data.wOffset % 4 == 0)
            {
                MessDiapatch::instance()->onFileTransStatusChanged(progress);
            }
        }
    }
}

#endif
