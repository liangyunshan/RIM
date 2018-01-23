#include "msgsender.h"

#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <qmath.h>

#include "netglobal.h"
#include "Util/rlog.h"

namespace ClientNetwork{

int SendPackId;
QMutex SendPackMutex;

#define MAGIC_NUM 0x7DBCD6AC

struct DataPacket
{
    unsigned int magicNum;                   //魔数，0x7DBCD6AC
    unsigned int packId;                     //数据包身份，一个id可对应多个帧，也可以有一个帧
    unsigned int currentIndex;               //当前帧
    unsigned int totalIndex;                 //总帧数量
    unsigned int currentLen;                 //当前数据长度
    unsigned int totalLen;                   //总数据长度
};

MsgSender::MsgSender(QThread *parent):tcpSocket(NULL),
   RTask(parent)
{
    SendPackId = 0;
}

void MsgSender::setSock(RSocket * sock)
{
    tcpSocket = sock;
}

void MsgSender::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
    else
    {
        G_SendWaitCondition.wakeOne();
    }
}

void MsgSender::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
        G_SendWaitCondition.wakeOne();
    }
}

void MsgSender::run()
{
    runningFlag = true;

    RLOG_INFO("Start Send++++++++++++++++++++++");
    while(runningFlag)
    {
        if(G_SendBuff.size() <= 0)
        {
            G_SendMutex.lock();
            G_SendWaitCondition.wait(&G_SendMutex);
            G_SendMutex.unlock();
        }
        if(runningFlag)
        {
            G_SendMutex.lock();
            QByteArray data =  G_SendBuff.dequeue();
            G_SendMutex.unlock();

            if(!handleDataSend(data))
            {
                emit socketError(tcpSocket->getLastError());
                break;
            }
        }
    }
    runningFlag = false;

    RLOG_INFO("Stop Send++++++++++++++++++++++");
}

/*!
     * @brief 向服务器发送指定数据
     * @details 此时需要在数据内容前加入数据头信息，用于控制分包发送时的序列
     * @param[in] data 待发送数据内容
     * @return 是否完全发送成功
     */
bool MsgSender::handleDataSend(QByteArray &data)
{
    if(tcpSocket->isValid())
    {
        DataPacket packet;
        memset((char *)&packet,0,sizeof(DataPacket));
        packet.magicNum = MAGIC_NUM;
        SendPackMutex.lock();
        packet.packId = ++SendPackId;
        SendPackMutex.unlock();

        packet.totalIndex = qCeil(((float)data.length() / MAX_PACKET));
        packet.totalLen = data.size();

        int sendLen = 0;

        qDebug()<<data.length()<<sizeof(DataPacket)<<packet.totalIndex;

        for(int i = 0; i < packet.totalIndex; i++)
        {
            memset(sendBuff,0,MAX_SEND_BUFF);
            packet.currentIndex = i;
            int leftLen = data.size() - sendLen;
            packet.currentLen = leftLen > MAX_PACKET ? MAX_PACKET: leftLen;

            memcpy(sendBuff,(char *)&packet,sizeof(DataPacket));
            memcpy(sendBuff + sizeof(DataPacket),data.data() + sendLen,packet.currentLen);

            int dataLen = sizeof(DataPacket)+packet.currentLen;
            int realSendLen = tcpSocket->send(sendBuff,dataLen);

            qDebug()<<"Send:"<<packet.currentIndex<<packet.currentLen<<packet.totalLen<<realSendLen<<sendLen;

            if(realSendLen == dataLen)
            {
                sendLen += packet.currentLen;
            }
            else
            {
                break;
            }
        }

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
