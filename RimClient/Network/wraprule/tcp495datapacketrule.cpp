#include "tcp495datapacketrule.h"

#include <qmath.h>
#include <QDebug>

#include "Network/network_global.h"

namespace ClientNetwork{

using namespace QDB495;

TCP495DataPacketRule::TCP495DataPacketRule():
    WrapRule()
{
    SendPackId = qrand()%1024;
}

void TCP495DataPacketRule::wrap(ProtocolPackage &data)
{

}

bool TCP495DataPacketRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    return false;
}

bool TCP495DataPacketRule::wrap(const ProtocolPackage &dataUnit, std::function<int (const char *, const int)> sendDataFunc)
{
    QDB495_SendPackage packet;
    memset((char *)&packet,0,sizeof(QDB495_SendPackage));
    packet.bVersion = 1;
    packet.bPackType = dataUnit.bPackType;
    packet.bPriority = 0;
    packet.bPeserve = dataUnit.bPeserve;
    packet.wSerialNo = ++SendPackId;
    packet.wCheckout = 0;
    packet.dwPackAllLen = dataUnit.data.size();
    packet.wDestAddr = dataUnit.wDestAddr;
    packet.wSourceAddr = dataUnit.wSourceAddr;

    int totalIndex = countTotoalIndex(dataUnit.data.length());
    int sendLen = 0;

    for(unsigned int i = 0; i < totalIndex; i++)
    {
        memset(sendBuff,0,MAX_SEND_BUFF);
        packet.wOffset = i;
        int leftLen = dataUnit.data.length() - sendLen;
        packet.wPackLen = leftLen > MAX_PACKET ? MAX_PACKET: leftLen;

        memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
        memcpy(sendBuff + sizeof(QDB495_SendPackage),dataUnit.data.data() + sendLen,packet.wPackLen);

        int dataLen = QDB495_SendPackage_Length+packet.wPackLen;
        int realSendLen = sendDataFunc(sendBuff,dataLen);

        if(realSendLen == dataLen){
            sendLen += packet.wPackLen;
        }else{
            break;
        }
    }

    if(sendLen == packet.dwPackAllLen){
        return true;
    }

    return false;
}

bool TCP495DataPacketRule::unwrap(const char *data, const int length, DataHandler recvDataFunc)
{
    RecvUnit result;
    result.extendData.method = C_TCP;

    if(lastRecvBuff.size() > 0)
    {
        int tmpBuffLen = lastRecvBuff.size() + length + 1;
        char * dataBuff = new char[tmpBuffLen];
        memset(dataBuff,0,tmpBuffLen * sizeof(char));

        memcpy(dataBuff,lastRecvBuff.data(),lastRecvBuff.size());
        memcpy(dataBuff + lastRecvBuff.size(),data,length);

        lastRecvBuff.clear();
        recvData(dataBuff,tmpBuffLen - 1,result);

        delete[] dataBuff;
    }
    else
    {
        recvData(data,length,result);
    }

    if(result.data.size() > 0){
        recvDataFunc(result);
        return true;
    }

    return false;
}

void TCP495DataPacketRule::recvData(const char *recvData, int recvLen, RecvUnit &result)
{
    QDB495_SendPackage packet;
    memset((char *)&packet,0,sizeof(QDB495_SendPackage));

    if(recvLen > sizeof(QDB495_SendPackage))
    {
        unsigned int processLen = 0;
        do
        {
            memcpy((char *)&packet,recvData+processLen,sizeof(QDB495_SendPackage));
            processLen += sizeof(QDB495_SendPackage);
            //[1]数据头部分正常
            if(true)
            {
                //[1.1]至少存在多余一个完整数据包
                if(packet.wPackLen <= recvLen - processLen)
                {
                    int totalIndex = countTotoalIndex(packet.dwPackAllLen);
                    //[1.1.1]一包数据
                    if(totalIndex == 1)
                    {
                        result.extendData.type495 = static_cast<PacketType_495>(packet.bPackType);
                        result.extendData.bPeserve = packet.bPeserve;
                        result.data.resize(packet.wPackLen);
                        memcpy(result.data.data(),recvData + processLen,packet.wPackLen);
                    }
                    //[1.1.2]多包数据(只保存数据部分)
                    else
                    {
                       QByteArray data;
                       data.resize(packet.wPackLen);
                       memcpy(data.data(),recvData + processLen,packet.wPackLen);

                       if(packetBuffs.value(packet.wSerialNo,NULL) == NULL)
                       {
                            PacketBuff * buff = new PacketBuff;
                            buff->totalPackIndex = totalIndex;
                            buff->recvPackIndex += 1;
                            buff->recvSize += packet.wPackLen;
                            buff->buff.append(data);

                            packetBuffs.insert(packet.wSerialNo,buff);
                       }
                       else
                       {
                            PacketBuff * buff = packetBuffs.value(packet.wSerialNo,NULL);
                            if(buff)
                            {
                                buff->buff.append(data);
                                buff->recvSize += packet.wPackLen;
                                buff->recvPackIndex += 1;
                                if(buff->recvPackIndex == buff->totalPackIndex)
                                {
                                    buff->isCompleted = true;

                                    result.extendData.type495 = static_cast<PacketType_495>(packet.bPackType);
                                    result.extendData.bPeserve = packet.bPeserve;
                                    result.data.append(buff->getFullData());

                                    packetBuffs.remove(packet.wSerialNo);
                                    delete buff;
                                }
                            }
                       }
                    }
                    processLen += packet.wPackLen;

                    //[1.1.3]
                    int leftLen = recvLen - processLen;

                    if(leftLen <= 0)
                        break;

                    if(leftLen >= sizeof(QDB495_SendPackage))
                    {
                        continue;
                    }
                    else
                    {
                        //[1.1.3.1]【信息被截断】
                        memcpy(&packet,recvData + processLen,leftLen);

                        lastRecvBuff.clear();
                        lastRecvBuff.append(recvData + processLen,leftLen);

                        processLen += leftLen;
                        break;
                    }

                }
                //[1.2]【信息被截断】
                else
                {
                    int leftLen = recvLen - processLen;

                    lastRecvBuff.clear();
                    lastRecvBuff.append((char *)&packet,sizeof(QDB495_SendPackage));
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
