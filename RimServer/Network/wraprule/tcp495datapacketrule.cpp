#include "tcp495datapacketrule.h"

#include <qmath.h>
#include <QDebug>

#include "head.h"
#include "../win32net/iocpcontext.h"
#include "../dataprocess/handler.h"

namespace ServerNetwork{

using namespace QDB495;

TCP495DataPacketRule::TCP495DataPacketRule():
    WrapRule(),handler(nullptr)
{
    SendPackId = qrand()%1024;
}

void TCP495DataPacketRule::registHandler(Handler *dataHandler)
{
    handler = dataHandler;
}

void TCP495DataPacketRule::bindContext(IocpContext * context, unsigned long recvLen)
{
    ioContext = context;

    ioContext->getPakcet()[recvLen] = 0;
    int lastRecvBuffSize = ioContext->getClient()->getHalfPacketBuff().size();

    if(lastRecvBuffSize > 0)
    {
        int tmpBuffLen = lastRecvBuffSize + recvLen + 1;
        char * dataBuff = new char[tmpBuffLen];
        memset(dataBuff,0,tmpBuffLen * sizeof(char));

        memcpy(dataBuff,ioContext->getClient()->getHalfPacketBuff().data(),lastRecvBuffSize);
        memcpy(dataBuff + lastRecvBuffSize,ioContext->getPakcet(),recvLen);

        ioContext->getClient()->getHalfPacketBuff().clear();

        recvData(dataBuff,lastRecvBuffSize + recvLen);

        delete[] dataBuff;
    }
    else
    {
        recvData(ioContext->getPakcet(),recvLen);
    }
}

void TCP495DataPacketRule::wrap(ProtocolPackage &data)
{

}

bool TCP495DataPacketRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    return false;
}


void TCP495DataPacketRule::recvData(const char *recvData, int recvLen)
{
    QDB495_SendPackage packet;
    memset((char *)&packet,0,sizeof(QDB495_SendPackage));

    if(recvLen >= sizeof(QDB495_SendPackage))
    {
        unsigned int processLen = 0;
        do
        {
            memcpy((char *)&packet,recvData+processLen,sizeof(QDB495_SendPackage));
            processLen += sizeof(QDB495_SendPackage);
            //[1]数据头部分正常
            if(true)
            {
                RecvUnit socketData;
                socketData.extendData.sockId = ioContext->getClient()->socket();
                socketData.extendData.type495 = static_cast<PacketType_495>(packet.bPackType);

                //[1.1]至少存在多余一个完整数据包
                if(packet.wPackLen <= recvLen - processLen)
                {
                    //[1.1.1]一包数据
                    int totalIndex = countTotoalIndex(packet.dwPackAllLen);
                    if(totalIndex == 1)
                    {
                        socketData.data.resize(packet.wPackLen);
                        memcpy(socketData.data.data(),recvData + processLen,packet.wPackLen);

                        if(handler)
                            handler->handle(socketData);
                    }
                    //[1.1.2]多包数据(只保存数据部分)
                    else
                    {
                       QByteArray data;
                       data.resize(packet.wPackLen);
                       memcpy(data.data(),ioContext->getPakcet() + processLen,packet.wPackLen);

                       std::unique_lock<std::mutex> ul(ioContext->getClient()->BuffMutex());
                       if(ioContext->getClient()->getPacketBuffs().value(packet.wSerialNo,NULL) == NULL)
                       {
                            PacketBuff * buff = new PacketBuff;
                            buff->totalPackIndex = totalIndex;
                            buff->recvPackIndex += 1;
                            buff->recvSize += packet.wPackLen;
                            buff->buff.append(data);

                            ioContext->getClient()->getPacketBuffs().insert(packet.wSerialNo,buff);
                       }
                       else
                       {
                            PacketBuff * buff = ioContext->getClient()->getPacketBuffs().value(packet.wSerialNo,NULL);
                            if(buff)
                            {
                                buff->buff.append(data);
                                buff->recvSize += packet.wPackLen;
                                buff->recvPackIndex += 1;
                                if(buff->recvPackIndex == buff->totalPackIndex)
                                {
                                    buff->isCompleted = true;

                                    socketData.data.append(buff->getFullData());

                                    ioContext->getClient()->getPacketBuffs().remove(packet.wSerialNo);
                                    delete buff;

                                    if(handler)
                                        handler->handle(socketData);
                                }
                            }
                       }
                    }

                    processLen += packet.wPackLen;

                    //[1.1.3]检验是否满足下次处理需求
                    int leftLen = recvLen - processLen;
                    if(leftLen <= 0)
                    {
                        break;
                    }

                    if(leftLen >= sizeof(QDB495_SendPackage))
                    {
                        continue;
                    }
                    else
                    {
                        //[1.1.3.1]【信息被截断】
                        memcpy(&packet,recvData + processLen,leftLen);

                        std::unique_lock<std::mutex> ul(ioContext->getClient()->BuffMutex());
                        ioContext->getClient()->getHalfPacketBuff().clear();
                        ioContext->getClient()->getHalfPacketBuff().append(recvData + processLen,leftLen);
                        processLen += leftLen;
                        break;
                    }
                }
                //[1.2]【信息被截断】
                else
                {
                    int leftLen = recvLen - processLen;

                    std::unique_lock<std::mutex> ul(ioContext->getClient()->BuffMutex());
                    ioContext->getClient()->getHalfPacketBuff().clear();
                    ioContext->getClient()->getHalfPacketBuff().append((char *)&packet,sizeof(QDB495_SendPackage));
                    ioContext->getClient()->getHalfPacketBuff().append(recvData+processLen,leftLen);

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
        std::unique_lock<std::mutex> ul(ioContext->getClient()->BuffMutex());
        ioContext->getClient()->getHalfPacketBuff().clear();
        ioContext->getClient()->getHalfPacketBuff().append((char *)recvData,recvLen);
    }
}

/*!
 * @brief 计算数据总分包数量
 * @param[in] totalLength 数据总长度
 * @return 总分包大小
 */
int TCP495DataPacketRule::countTotoalIndex(const int totalLength)
{
    return qCeil(((float) totalLength/ MAX_PACKET));
}

}
