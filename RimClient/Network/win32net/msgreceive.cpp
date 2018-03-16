#include "msgreceive.h"

#include <QUdpSocket>
#include <QDebug>

#include "Util/rlog.h"
#include "Util/rutil.h"
#include "netglobal.h"

#define MSG_RECV_BUFF 1024

namespace ClientNetwork{

RecveiveTask::RecveiveTask(QObject *parent) :
    tcpSocket(NULL),RTask(parent)
{

}

void RecveiveTask::setSock(RSocket *sock)
{
    tcpSocket = sock;
}

void RecveiveTask::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
}

void RecveiveTask::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
    }
}

void RecveiveTask::run()
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

                recvData(dataBuff,lastRecvBuff.size() + recvLen);
                lastRecvBuff.clear();

                delete[] dataBuff;
            }
            else
            {
                recvData(recvBuff,recvLen);
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

void RecveiveTask::recvData(char * recvData,int recvLen)
{
    DataPacket packet;
    memset((char *)&packet,0,sizeof(DataPacket));

    if(recvLen > sizeof(DataPacket))
    {
        unsigned int processLen = 0;
        do
        {
            memcpy((char *)&packet,recvData,sizeof(DataPacket));
            processLen += sizeof(DataPacket);

            //[1]数据头部分正常
            if(packet.magicNum == RECV_MAGIC_NUM)
            {
                QByteArray dataBuff;
                //[1.1]至少存在多余一个完整数据包
                if(packet.currentLen <= recvLen - processLen)
                {
                    //[1.1.1]一包数据
                    if(packet.totalIndex == 1)
                    {
                        dataBuff.resize(packet.currentLen);
                        memcpy(dataBuff.data(),recvData + processLen,packet.currentLen);

                        processData(dataBuff);
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

                                    dataBuff.append(buff->getFullData());

                                    packetBuffs.remove(packet.packId);
                                    delete buff;

                                    processData(dataBuff);
                                }
                            }
                       }
                    }
                    processLen += packet.currentLen;

                    //[1.1.3]
                    int leftLen = recvLen - processLen;

                    if(leftLen <= 0)
                    {
                        break;
                    }

                    if(leftLen >= sizeof(DataPacket))
                    {
                        continue;
                    }
                    else
                    {
                        //[1.1.3.1]【信息被截断】
                        memcpy(&packet,recvData + processLen,leftLen);

                        lastRecvBuff.clear();
                        lastRecvBuff.append((char *)&packet,leftLen);

                        processLen += leftLen;
                        break;
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
                    break;
                }
            }
            else
            {
                qDebug()<<"Recv Error Packet";
            }
        }while(processLen <= recvLen);
    }
    else
    {
        lastRecvBuff.clear();
        lastRecvBuff.append(recvData,recvLen);
    }
}

RecveiveTask::~RecveiveTask()
{

}

TextReceive::TextReceive(QObject *parent):
    RecveiveTask(parent)
{

}

TextReceive::~TextReceive()
{

}

void TextReceive::processData(QByteArray &data)
{
    G_TextRecvMutex.lock();
    G_TextRecvBuffs.enqueue(data);
    G_TextRecvMutex.unlock();

    G_TextRecvCondition.wakeOne();
}

FileReceive::FileReceive(QObject *parent):
    RecveiveTask(parent)
{

}

FileReceive::~FileReceive()
{

}

void FileReceive::processData(QByteArray &data)
{
    G_FileRecvMutex.lock();
    G_FileRecvBuffs.enqueue(data);
    G_FileRecvMutex.unlock();

    G_FileRecvCondition.wakeOne();
}

} //namespace ClientNetwork;
