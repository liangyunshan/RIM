#include "msgreceive.h"

#include <QUdpSocket>
#include <QDebug>

#include "Util/rlog.h"
#include "Util/rutil.h"
#include "netglobal.h"

#define MSG_RECV_BUFF 1024

namespace ClientNetwork{

MsgReceive::MsgReceive(QObject *parent) :
    tcpSocket(NULL),RTask(parent)
{

}

MsgReceive::~MsgReceive()
{

}

void MsgReceive::setSock(RSocket *sock)
{
    tcpSocket = sock;
}

void MsgReceive::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
}

void MsgReceive::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
    }
}

void MsgReceive::run()
{
    RLOG_INFO("Start Receive++++++++++++++++++++++");

    runningFlag = true;
    lastRecvBuff.clear();

    char recvBuff[MSG_RECV_BUFF] = {0};

    while(runningFlag)
    {
        memset(recvBuff,0,MSG_RECV_BUFF);
        int recvLen = tcpSocket->recv(recvBuff,MSG_RECV_BUFF);
        if(recvLen > 0)
        {
            if(lastRecvBuff.size() > 0)
            {
                int tmpBuffLen = lastRecvBuff.size() + recvLen + 1;
                char * dataBuff = new char[tmpBuffLen];
                memset(dataBuff,0,tmpBuffLen * sizeof(char));

                memcpy(dataBuff,lastRecvBuff.data(),lastRecvBuff.size());
                memcpy(dataBuff + lastRecvBuff.size(),recvBuff,recvLen);


                processRecvData(dataBuff,lastRecvBuff.size() + recvLen);
                lastRecvBuff.clear();

                delete[] dataBuff;
            }
            else
            {
                processRecvData(recvBuff,recvLen);
            }
        }
        else
        {
            RLOG_ERROR("receive a pack not completely %d",tcpSocket->getLastError());
            tcpSocket->closeSocket();
            emit socketError(tcpSocket->getLastError());
            break;
        }
    }

    runningFlag = false;
    RLOG_INFO("Stop Receive++++++++++++++++++++++");
}

void MsgReceive::processRecvData(char * recvData,int recvLen)
{
    DataPacket packet;
    memset((char *)&packet,0,sizeof(DataPacket));

    if(recvLen > sizeof(DataPacket))
    {
        memcpy((char *)&packet,recvData,sizeof(DataPacket));
        //[1]数据头部分正常
        if(packet.magicNum == RECV_MAGIC_NUM)
        {
            int processLen = sizeof(DataPacket);
            do
            {
                QByteArray processData;
                //[1.1]至少存在多余一个完整数据包
                if(packet.currentLen <= recvLen - processLen)
                {
                    //[1.1.1]一包数据
                    if(packet.totalIndex == 1)
                    {
                        processData.resize(packet.currentLen);
                        memcpy(processData.data(),recvData + processLen,packet.currentLen);

                        G_RecvMutex.lock();
                        G_RecvButts.enqueue(processData);
                        G_RecvMutex.unlock();

                        G_RecvCondition.wakeOne();
                    }
                    //[1.1.2]多包数据(只保存数据部分)
                    else
                    {
                       QByteArray data;
                       data.resize(packet.currentLen);
                       memcpy(data.data(),recvData + processLen,packet.currentLen);

                       if(packetBuffs.value(packet.packId,NULL) == NULL)
                       {
                            PacketBuff * buff = new PacketBuff;
                            buff->totalPackIndex = packet.totalIndex;
                            buff->recvPackIndex += 1;
                            buff->recvSize += packet.currentLen;
                            buff->buff.append(data);

                            packetBuffs.insert(packet.packId,buff);
                       }
                       else
                       {
                            PacketBuff * buff = packetBuffs.value(packet.packId,NULL);
                            if(buff)
                            {
                                buff->buff.append(data);
                                buff->recvSize += packet.currentLen;
                                buff->recvPackIndex += 1;
                                if(buff->recvPackIndex == buff->totalPackIndex)
                                {
                                    buff->isCompleted = true;

                                    processData.append(buff->getFullData());

                                    packetBuffs.remove(packet.packId);
                                    delete buff;

                                    G_RecvMutex.lock();
                                    G_RecvButts.enqueue(processData);
                                    G_RecvMutex.unlock();

                                    G_RecvCondition.wakeOne();
                                }
                            }
                       }
                    }
                    processLen += packet.currentLen;

                    //[1.1.3]
                    int leftLen = recvLen - processLen;

                    if(leftLen == 0)
                    {
                        break;
                    }

                    if(leftLen >= sizeof(DataPacket))
                    {
                        memcpy(&packet,recvData + processLen,sizeof(DataPacket));
                        processLen += sizeof(DataPacket);
                    }
                    else
                    {
                        //[1.1.3.1]【信息被截断】
                        memcpy(&packet,recvData + processLen,leftLen);

                        lastRecvBuff.clear();
                        lastRecvBuff.append((char *)&packet,leftLen);

                        processLen += leftLen;
                    }
                }
                //[1.2]【信息被截断】
                else
                {
                    int leftLen = recvLen - processLen;
                    lastRecvBuff.clear();
                    lastRecvBuff.append((char *)&packet,sizeof(DataPacket));
                    lastRecvBuff.append(recvData+processLen,leftLen);

                    processLen += leftLen;
                }

            }while(processLen < recvLen);
        }
        else
        {
            qDebug()<<"Recv Error Packet";
        }
    }
}

} //namespace ClientNetwork;
