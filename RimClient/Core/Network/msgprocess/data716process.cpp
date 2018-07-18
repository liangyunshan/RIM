#include "data716process.h"

#ifdef __LOCAL_CONTACT__

#include <QDebug>
#include <qmath.h>

#include "util/rsingleton.h"
#include "messdiapatch.h"
#include "util/rsingleton.h"
#include "global.h"
#include "file/filedesc.h"
#include "file/filemanager.h"
#include "Network/msgwrap/wrapfactory.h"
#include "Network/network_global.h"
#include "Util/rutil.h"
#include "../../user/user.h"

using namespace QDB495;
using namespace QDB21;
using namespace QDB2051;

/*!
 * @brief 查询指定节点的通信配置方式
 * @param[in] nodeId 带查询的节点号
 * @param[in] success 是否查询成功的标识
 */
OuterNetConfig QueryNodeConfig(unsigned short nodeId,bool & success )
{
    success = false;

    if(G_ParaSettings){
        auto index = std::find_if(G_ParaSettings->outerNetConfig.begin(),G_ParaSettings->outerNetConfig.end(),[&nodeId](const OuterNetConfig & conf){
            return QString::number(nodeId) == conf.nodeId;
        });

        if(index != G_ParaSettings->outerNetConfig.end()){
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
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
           <<"I recv a 2048 affirm data:"<<data.usOrderNo<<data.usSerialNo
          <<"\n";
    if(data.usOrderNo == O_2048)
    {
        TextReply textReply;
        textReply.wSourceAddr = data.wSourceAddr;
        textReply.wDestAddr = data.wDestAddr;
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
    response.accountId = QString::number(data.wDestAddr);
    response.textId = QString::number(data.usSerialNo);
    response.otherSideId = QString::number(data.wSourceAddr);
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
        request.sourceId = QString::number(data.wDestAddr);
        request.destId = QString::number(data.wSourceAddr);
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
               <<"ready affirm data:"<<request.extendData.usOrderNo<<request.extendData.usSerialNo
              <<"\n";
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
    std::shared_ptr<FileDesc> fileDesc = RSingleton<FileManager>::instance()->get716File(QString::number(data.wSourceAddr)
                                                ,data.usSerialNo,QString::fromLocal8Bit(data.cFilename));
    if(fileDesc.get() == nullptr)
    {
        fileDesc = std::make_shared<FileDesc>();

        fileDesc->setFilePath(G_User->getC2CFilePath());

        fileDesc->md5 = RUtil::UUID();
        fileDesc->itemKind = (int)data.cFileType;
        fileDesc->bPackType = data.bPackType;
        fileDesc->fileName = QString::fromLocal8Bit(data.cFilename);
        fileDesc->accountId = QString::number(data.wSourceAddr);
        fileDesc->otherId = QString::number(data.wDestAddr);
        fileDesc->usOrderNo = data.usOrderNo;
        fileDesc->usSerialNo = data.usSerialNo;
        fileDesc->fileId = QString("%1_%2_%3").arg(data.wSourceAddr).arg(data.usSerialNo).arg(QString::fromLocal8Bit(data.cFilename));

        int protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2051_Head_Length;
        int fileHeadLen = protocolDataLen + data.cFilename.size();
        int sliceNums = qCeil((float)data.dwPackAllLen /(fileHeadLen + MAX_PACKET));

        fileDesc->size = data.dwPackAllLen - sliceNums * fileHeadLen ;
        fileDesc->writeLen = 0;

        if(!fileDesc->create())
            return;

        if(!RSingleton<FileManager>::instance()->addFile(fileDesc))
            return;
    }

    if(fileDesc->seek(data.wOffset * MAX_PACKET) && fileDesc->write(data.data) > 0)
    {
        if(fileDesc->flush() && fileDesc->isRecvOver())
        {
            fileDesc->close();

            FileRecvDesc fdesc;
            fdesc.usSerialNo = fileDesc->usSerialNo;
            fdesc.wSourceAddr = fileDesc->accountId.toUShort();
            fdesc.wDestAddr = fileDesc->otherId.toUShort();
            fdesc.fileName = fileDesc->fileName;

            //TODO 对接收文件进行处理
            MessDiapatch::instance()->onRecvFileData(fdesc);
            RSingleton<FileManager>::instance()->removeFile(fileDesc->fileId);
        }
    }
}

#endif
