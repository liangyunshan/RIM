#include "tcpdatapacketrule.h"
#include <qmath.h>
#include <QDebug>

namespace ClientNetwork{

TCPDataPacketRule::TCPDataPacketRule():
    WrapRule()
{
    SendPackId = qrand()%1024 + 1000;
}

QByteArray TCPDataPacketRule::wrap(const ProtocolPackage &data)
{
    return QByteArray(data.cFileData);
}

/*!
 * @brief 封装发送数据，并传递发送到方式
 * @param[in] data 待发送的数据
 * @param[in] func 发送数据函数
 * @return 返回是否发送成功
 */
bool TCPDataPacketRule::wrap(const QByteArray &data, std::function<int (const char *, const int)> sendDataFunc)
{
    DataPacket packet;
    memset((char *)&packet,0,sizeof(DataPacket));
    packet.magicNum = SEND_MAGIC_NUM;
    packet.packId = ++SendPackId;
    packet.totalIndex = qCeil(((float)data.length() / MAX_PACKET));
    packet.totalLen = data.length();

    int sendLen = 0;

    for(unsigned int i = 0; i < packet.totalIndex; i++)
    {
        memset(sendBuff,0,MAX_SEND_BUFF);
        packet.currentIndex = i;
        int leftLen = data.length() - sendLen;
        packet.currentLen = leftLen > MAX_PACKET ? MAX_PACKET: leftLen;

        memcpy(sendBuff,(char *)&packet,sizeof(DataPacket));
        memcpy(sendBuff + sizeof(DataPacket),data.data() + sendLen,packet.currentLen);

        int dataLen = sizeof(DataPacket)+packet.currentLen;
        int realSendLen = sendDataFunc(sendBuff,dataLen);

        if(realSendLen == dataLen){
            sendLen += packet.currentLen;
        }else{
            break;
        }
    }

    if(sendLen == packet.totalLen){
        return true;
    }
    return false;
}

QByteArray TCPDataPacketRule::unwrap(const QByteArray &data)
{
    return QByteArray(data);
}

/*!
 * @brief 将接收的数据根据协议进行解析，返回解析后的结果数据
 * @param[in] data 待处理的接收数据
 * @param[in] length 数据长度
 * @return 解析后的数据
 */
bool TCPDataPacketRule::unwrap(const char *data, const int length, ByteArrayHandler recvDataFunc)
{
    QByteArray result;
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

    if(result.size() > 0){
        recvDataFunc(result);
        return true;
    }

    return false;
}

void TCPDataPacketRule::recvData(const char * recvData,int recvLen,QByteArray & result)
{
    DataPacket packet;
    memset((char *)&packet,0,sizeof(DataPacket));

    if(recvLen > sizeof(DataPacket))
    {
        unsigned int processLen = 0;
        do
        {
            memcpy((char *)&packet,recvData+processLen,sizeof(DataPacket));
            processLen += sizeof(DataPacket);
            //[1]数据头部分正常
            if(packet.magicNum == RECV_MAGIC_NUM)
            {
                //[1.1]至少存在多余一个完整数据包
                if(packet.currentLen <= recvLen - processLen)
                {
                    //[1.1.1]一包数据
                    if(packet.totalIndex == 1)
                    {
                        result.resize(packet.currentLen);
                        memcpy(result.data(),recvData + processLen,packet.currentLen);
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

                                    result.append(buff->getFullData());

                                    packetBuffs.remove(packet.packId);
                                    delete buff;
                                }
                            }
                       }
                    }
                    processLen += packet.currentLen;

                    //[1.1.3]
                    int leftLen = recvLen - processLen;

                    if(leftLen <= 0)
                        break;

                    if(leftLen >= sizeof(DataPacket))
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


} // namespace ClientNetwork


