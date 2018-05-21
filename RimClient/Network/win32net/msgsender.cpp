#include "msgsender.h"

#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <qmath.h>

#include "netglobal.h"
#include "Util/rlog.h"

namespace ClientNetwork{

SendTask::SendTask(QThread *parent):tcpSocket(NULL),
    RTask(parent)
{
    SendPackId = qrand()%1024 + 1000;
}

void SendTask::setSock(RSocket * sock)
{
    tcpSocket = sock;
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
            QByteArray data =  G_TextSendBuffs.dequeue();
            G_TextSendMutex.unlock();

            if(!handleDataSend(data))
            {
                emit socketError(tcpSocket->getLastError());
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
bool TextSender::handleDataSend(QByteArray &data)
{
    if(tcpSocket->isValid())
    {
        DataPacket packet;
        memset((char *)&packet,0,sizeof(DataPacket));
        packet.magicNum = SEND_MAGIC_NUM;
        SendPackMutex.lock();
        packet.packId = ++SendPackId;
        SendPackMutex.unlock();

        packet.totalIndex = qCeil(((float)data.length() / MAX_PACKET));
        packet.totalLen = data.size();

        int sendLen = 0;

        for(unsigned int i = 0; i < packet.totalIndex; i++)
        {
            memset(sendBuff,0,MAX_SEND_BUFF);
            packet.currentIndex = i;
            int leftLen = data.size() - sendLen;
            packet.currentLen = leftLen > MAX_PACKET ? MAX_PACKET: leftLen;

            memcpy(sendBuff,(char *)&packet,sizeof(DataPacket));
            memcpy(sendBuff + sizeof(DataPacket),data.data() + sendLen,packet.currentLen);

            int dataLen = sizeof(DataPacket)+packet.currentLen;
            int realSendLen = tcpSocket->send(sendBuff,dataLen);

qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<packet.currentIndex<<packet.currentLen<<packet.totalLen<<realSendLen<<sendLen;

            if(realSendLen == dataLen){
                sendLen += packet.currentLen;
            }else{
                break;
            }
        }

        if(sendLen == packet.totalLen){
            return true;
        }
    }

    RLOG_ERROR("Send a data error!");
    tcpSocket->closeSocket();

    return false;
}

FileSender::FileSender(QThread *parent):
    SendTask(parent)
{

}

FileSender::~FileSender()
{
    stopMe();
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
            {
                break;
            }
            G_FileSendMutex.lock();
            G_FileSendWaitCondition.wait(&G_FileSendMutex);
            G_FileSendMutex.unlock();
        }

        if(runningFlag)
        {
            G_FileSendMutex.lock();
            QByteArray data =  G_FileSendBuffs.dequeue();
            G_FileSendMutex.unlock();

            if(!handleDataSend(data))
            {
                emit socketError(tcpSocket->getLastError());
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
bool FileSender::handleDataSend(QByteArray &data)
{
    if(tcpSocket->isValid())
    {
        DataPacket packet;
        memset((char *)&packet,0,sizeof(DataPacket));
        packet.magicNum = SEND_MAGIC_NUM;
        SendPackMutex.lock();
        packet.packId = ++SendPackId;
        SendPackMutex.unlock();

        packet.totalIndex = qCeil(((float)data.length() / MAX_PACKET));
        packet.totalLen = data.size();

        int sendLen = 0;

        for(unsigned int i = 0; i < packet.totalIndex; i++)
        {
            memset(sendBuff,0,MAX_SEND_BUFF);
            packet.currentIndex = i;
            int leftLen = data.size() - sendLen;
            packet.currentLen = leftLen > MAX_PACKET ? MAX_PACKET: leftLen;

            memcpy(sendBuff,(char *)&packet,sizeof(DataPacket));
            memcpy(sendBuff + sizeof(DataPacket),data.data() + sendLen,packet.currentLen);

            int dataLen = sizeof(DataPacket)+packet.currentLen;
            int realSendLen = tcpSocket->send(sendBuff,dataLen);

//            qDebug()<<"Send FileSender:"<<packet.currentIndex<<packet.currentLen<<packet.totalLen<<realSendLen<<sendLen<<sendBuff;

            if(realSendLen == dataLen)
            {
                sendLen += packet.currentLen;
            }
            else
            {
                break;
            }
        }

        msleep(10);

        if(sendLen == packet.totalLen)
        {
            return true;
        }
    }

    RLOG_ERROR("Send a data error!");
    tcpSocket->closeSocket();

    return false;
}

} //ClientNetwork
