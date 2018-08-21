#include "format495function.h"
#include "others/serialno.h"
#include "util/rsingleton.h"
#include "../../protocol/datastruct.h"
#include "../../protocol/protocoldata.h"
#include "global.h"
#include "user/user.h"
#include "Util/scaleswitcher.h"
#include "Network/msgwrap/wrapfactory.h"

#include <cstdarg>
#include <QFileInfo>

Format495Function::Format495Function()
{

}

/*!
 * @brief 发送注册报
 */
void Format495Function::sendRegistPack()
{
    DataPackType request;
    request.msgType = MSG_CONTROL;
    request.msgCommand = MSG_TCP_TRANS;
    request.extendData.type495 = T_DATA_REG;
    request.extendData.usOrderNo = O_NONE;
    request.extendData.usSerialNo = SERIALNO_FRASH;
    request.sourceId = G_User->BaseInfo().accountId;
    request.destId = request.sourceId;
    //同时注册多个地址(暂时为一个)
    addRegistNode(request.extendData.data,1,ScaleSwitcher::fromHexToDec(request.sourceId));
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,C_TongKong,M_495);
}

/*!
 * @brief 发送注销报
 */
void Format495Function::sendUnRegistPack()
{
    DataPackType request;
    request.msgType = MSG_CONTROL;
    request.msgCommand = MSG_TCP_TRANS;
    request.extendData.type495 = T_DATA_REG;
    request.extendData.usOrderNo = O_NONE;
    request.extendData.usSerialNo = SERIALNO_FRASH;
    request.sourceId = G_User->BaseInfo().accountId;
    request.destId = request.sourceId;
    for(int i=0;i<4;i++)
    {
        request.extendData.data.append(char(0));
    }
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,C_TongKong,M_495);
}

/*!
 * @brief 发送自身在线测试报
 * @note 定时发送的报文
 */
void Format495Function::sendTestSelfOnlinePack()
{
    DataPackType request;
    request.msgType = MSG_CONTROL;
    request.msgCommand = MSG_TCP_TRANS;
    request.extendData.type495 = T_CONNECT_TEST;
    request.extendData.usOrderNo = O_NONE;
    request.extendData.usSerialNo = 0;
    request.sourceId = QString::number(0);
    request.destId = QString::number(0);
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,C_TongKong,M_495);
}

/*!
 * @brief 发送测试报
 */
void Format495Function::sendTestPack(QString destId)
{
    DataPackType request;
    request.msgType = MSG_CONTROL;
    request.msgCommand = MSG_TCP_TRANS;
    request.extendData.type495 = T_TEST;
    request.extendData.usOrderNo = O_NONE;
    request.extendData.usSerialNo = 0;
    request.sourceId = G_User->BaseInfo().accountId;
    request.destId = destId;
    for(int i=0;i<10;i++)
    {
        request.extendData.data.append(char(1));
    }
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,C_TongKong,M_495);
}

/*!
 * @brief 发送测试结果报
 */
void Format495Function::sendTestResultPack(QString destId)
{
    DataPackType request;
    request.msgType = MSG_CONTROL;
    request.msgCommand = MSG_TCP_TRANS;
    request.extendData.type495 = T_TEST_RESULT;
    request.extendData.usOrderNo = O_NONE;
    request.extendData.usSerialNo = 0;
    request.sourceId = G_User->BaseInfo().accountId;
    request.destId = destId;
    request.extendData.data.append(char(1));
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(&request,C_TongKong,M_495);
}

/*!
 * @brief 添加注册节点
 * @param[in/out] data 保存格式化后的结果信息
 * @param[in] addr 待一并注册的地址信息
 */
void Format495Function::addRegistNode(QByteArray & data,unsigned short nodeNums...)
{
    if(nodeNums <= 0)
        return;

    data.append((char)0);
    data.append((unsigned char)nodeNums);

    std::va_list list;
    va_start(list,nodeNums);
    for(unsigned short i = 0; i < nodeNums;i++){
        unsigned short tmpNode = va_arg(list,unsigned short);
        data.append((char *)&tmpNode,sizeof(unsigned short));
    }
    va_end(list);
}

/*!
 * @brief 判断文件是否可以通过495链路发送
 * @param filePath 文件绝对路径
 * @return  判断结果为true:文件可以被发送 false:文件不能发送
 */
bool Format495Function::checkFileCanbeSend(QString filePath)
{
    QFileInfo info(filePath);
    return checkFileCanbeSend(info.size());
}

/*!
 * @brief 判断数据是否可以通过495链路发送
 * @param fileSize 数据大小
 * @return 判断结果为true:数据可以被发送 false:数据不能发送
 */
bool Format495Function::checkFileCanbeSend(int fileSize)
{
    if( fileSize<=(MAX_PACKET * 65535) )
        return true;
    return false;
}
