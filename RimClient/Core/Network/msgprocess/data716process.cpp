#include "data716process.h"

#ifdef __LOCAL_CONTACT__

#include "util/rsingleton.h"
#include "messdiapatch.h"
#include "util/rsingleton.h"
#include "Network/msgwrap/wrapfactory.h"
#include "global.h"
#include <QDebug>

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
    response.timeStamp = data.usSerialNo;
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
        RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,conf.communicationMethod,conf.messageFormat);
    }
}

void Data716Process::processTextApply(ProtocolPackage &data)
{

}

#endif
