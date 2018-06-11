#include "msgsender.h"

#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <qmath.h>

#include "netglobal.h"
#include "Util/rlog.h"
#include "../multitransmits/basetransmit.h"

//716-TK兼容
#include "rudpsocket.h"
//

namespace ClientNetwork{

SendTask::SendTask(QThread *parent):
    RTask(parent)
{
    //716-TK
    m_pRUDPSendSocket = new RUDPSocket(QHostAddress::AnyIPv4,7755);
    //
}

bool SendTask::addTransmit(CommMethod method, BaseTransmit *trans)
{
    if(trans == nullptr)
        return false;

    std::lock_guard<std::mutex> lg(tranMutex);
    transmits.insert(method,trans);
    return true;
}

void SendTask::run()
{
    runningFlag = true;

    RLOG_INFO("Start Send++++++++++++++++++++++");
    processData();
    runningFlag = false;

    RLOG_INFO("Stop Send++++++++++++++++++++++");
}

TextSender::TextSender(QThread *parent):
   SendTask(parent)
{

}

TextSender::~TextSender()
{
    stopMe();
    wait();
}

void TextSender::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
    else
    {
        G_TextSendWaitCondition.wakeOne();
    }
}

void TextSender::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
        G_TextSendWaitCondition.wakeOne();
    }
}

void TextSender::processData()
{
    while(runningFlag)
    {
        while(runningFlag && G_TextSendBuffs.size() <= 0)
        {
            G_TextSendMutex.lock();
            G_TextSendWaitCondition.wait(&G_TextSendMutex);
            G_TextSendMutex.unlock();
        }
        if(runningFlag)
        {
            G_TextSendMutex.lock();
            SendUnit unit =  G_TextSendBuffs.dequeue();
            G_TextSendMutex.unlock();

            if(!handleDataSend(unit))
            {
                emit socketError(unit.method);
                break;
            }
        }
    }
}

/*!
 * @brief 向服务器发送指定数据
 * @details 此时需要在数据内容前加入数据头信息，用于控制分包发送时的序列
 * @param[in] data 待发送数据内容
 * @return 是否完全发送成功
 */
bool TextSender::handleDataSend(SendUnit &unit)
{
    std::lock_guard<std::mutex> lg(tranMutex);

//<<<<<<< HEAD:RimClient/Network/win32net/tcpmsgsender.cpp
//    }else if(unit.method == C_UDP){
//        QHostAddress destAddr("127.0.0.1");
//        quint16 destPort = 7755;
//        if(m_pRUDPSendSocket)
//        {
//            m_pRUDPSendSocket->SendData(false,unit.data,unit.data.size(),destAddr,destPort);
//        }
//    }else if(unit.method == C_BUS){

//    }
//=======
    BaseTransmit * trans = transmits.value(unit.method);
    if(trans != nullptr)
        return trans->startTransmit(unit);
    return false;
}

FileSender::FileSender(QThread *parent):
    SendTask(parent)
{
    dataPacketRule = std::make_shared<DataPacketRule>();
}

FileSender::~FileSender()
{
    stopMe();
    dataPacketRule.reset();
    wait();
}

void FileSender::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
    else
    {
        G_FileSendWaitCondition.wakeOne();
    }
}

void FileSender::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
        G_FileSendWaitCondition.wakeOne();
    }
}

void FileSender::processData()
{
    while(runningFlag)
    {
        while(G_FileSendBuffs.size() <= 0)
        {
            if(!runningFlag)
                break;

            G_FileSendMutex.lock();
            G_FileSendWaitCondition.wait(&G_FileSendMutex);
            G_FileSendMutex.unlock();
        }

        if(runningFlag)
        {
            G_FileSendMutex.lock();
            SendUnit sendUnit =  G_FileSendBuffs.dequeue();
            G_FileSendMutex.unlock();

            if(!handleDataSend(sendUnit))
            {
                emit socketError(sendUnit.method);
                break;
            }
        }
    }
}

/*!
 * @brief 处理数据发送
 * @details 网络层接收应用层传输数据后，在数据的前增加了网络层的标识，负责网络层数据的可靠投递; @n
 *          每次发送时会将应用层数据分配一个自增的ID(SendPackId)，用于表示应用层数据的身份信息(不管当前数据包长度的大小); @n
 *          若应用层数据包，超过了网络层一次传输的阈值，则对该应用层数据进行分子包传输，每个子包按照从0开始自增索引，用于控制传输的序列; @n
 * @param[in] data 待发送的数据包信息
 * @return 是否插入成功
 */
bool FileSender::handleDataSend(SendUnit &unit)
{
    std::lock_guard<std::mutex> lg(tranMutex);

    BaseTransmit * trans = transmits.value(unit.method);
    if(trans != nullptr)
        return trans->startTransmit(unit);

    return false;
}

} //ClientNetwork
