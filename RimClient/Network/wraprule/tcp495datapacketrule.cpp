#include "tcp495datapacketrule.h"

#ifdef __LOCAL_CONTACT__

#include <qmath.h>
#include <QDebug>

#include "Network/network_global.h"
#include "Util/rsingleton.h"
#include "tcp_wraprule.h"
#ifdef Q_OS_WIN
#include <WinSock2.h>
#include <Windows.h>
#endif

namespace ClientNetwork{

using namespace QDB495;
using namespace QDB21;
using namespace QDB2051;
using namespace QDB2048;


unsigned short TransferAddr(QByteArray str)
{
    int i;
    int k[4];
    int Digits=str.length();
    for(i=0;i<Digits;i++)
    {
        if(str[i]>='0'&&str[i]<='9')
            k[i]=str[i]-48;
        else if(str[i]>='A'&&str[i]<='F')
            k[i]=str[i]-'A'+10;
        else if(str[i]>='a'&&str[i]<='f')
            k[i]=str[i]-'a'+10;

    }
    unsigned short tmp = ((unsigned short)(k[0]*4096+k[1]*256+k[2]*16+k[3]));
    return tmp;
}

QByteArray UnTransferAddr(unsigned short addr)
{
    char addrList[5];
    unsigned char temp[4];
    temp[0] = addr / 4096;
    temp[1] = (addr % 4096) / 256;
    temp[2] = ((addr % 4096) % 256) / 16;
    temp[3] = ((addr % 4096) % 256) % 16;

    for(int i=0; i<4; i++)
    {
        if(temp[i] < 10)
        {
            addrList[i] = char(temp[i] + 48);
        }
        else
        {
            addrList[i] = char(temp[i] + 87);
        }
    }

    addrList[4] = char(0);
    return QByteArray(addrList,5);
}

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

bool TCP495DataPacketRule::wrap(ProtocolPackage &dataUnit, std::function<int (const char *, const int)> sendDataFunc)
{
    QDB495_SendPackage packet;
    memset((char *)&packet,0,sizeof(QDB495_SendPackage));
    packet.bVersion = 1;
    packet.bPackType = dataUnit.bPackType;
    packet.bPriority = 0;
    packet.bPeserve = htons(dataUnit.bPeserve);
    packet.wSerialNo = dataUnit.usSerialNo;
    packet.wCheckout = 0;
    bool ret = true;
//    unsigned short wSourceAddr = QString("0x%1").arg(dataUnit.wSourceAddr).toUShort(&ret,16);
    unsigned short wSourceAddr = TransferAddr(QString("%1").arg(dataUnit.wSourceAddr).toLatin1());
    packet.wSourceAddr = htons(wSourceAddr);
//    unsigned short wDestAddr = QString("0x%1").arg(dataUnit.wDestAddr).toUShort(&ret,16);
    unsigned short wDestAddr = TransferAddr(QString("%1").arg(dataUnit.wDestAddr).toLatin1());
    packet.wDestAddr = htons(wDestAddr);

    int sendDataLen = 0;

    //TODO:接入原有服务器 尚超 20180726
    //[1]
    if(packet.bPackType == WM_DATA_REG)
    {
        packet.wPackLen = htons(10);
        packet.dwPackAllLen = packet.wPackLen;
        packet.wOffset = 0;
        char buff[10];
        memset(buff,0,10);
        buff[0] = 0;
        buff[1] = 4;

        packet.wSourceAddr=htons(packet.wSourceAddr);
        memcpy(buff+sizeof(char)*2,(char *)&packet.wSourceAddr,sizeof(char)*2);

        memset(sendBuff,0,TCP_SEND_BUFF);
        memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
        memcpy(sendBuff + sizeof(QDB495_SendPackage),buff,10);
        int realSendLen = sendDataFunc(sendBuff,QDB495_SendPackage_Length + 10);

        if(realSendLen == packet.wPackLen){
            return true;
        }
        return true;
    }
    else
    {
        QByteArray originalData = dataUnit.data;
        //多个协议头长度
        int protocolDataLen = 0;
        if(dataUnit.usOrderNo == O_2051){
            protocolDataLen = QDB21_Head_Length + QDB2051_Head_Length;
        }else if(dataUnit.usOrderNo == 2048){
            protocolDataLen = QDB21_Head_Length + QDB2048_Head_Length;
        }
        int totalIndex = countTotoalIndex(originalData.length());
        packet.dwPackAllLen = htonl(originalData.length() + totalIndex * protocolDataLen);

        for(unsigned int i = 0; i < totalIndex; i++)
        {
            packet.wOffset = htons(i);

            int leftDataLen = originalData.length() - sendDataLen;
            int sliceLen = leftDataLen > MAX_PACKET ? MAX_PACKET: leftDataLen;

            dataUnit.data.clear();
            dataUnit.data.append(originalData.mid(sendDataLen,sliceLen));
            RSingleton<TCP_WrapRule>::instance()->wrap(dataUnit);

            packet.wPackLen = htons(protocolDataLen + sliceLen);

            memset(sendBuff,0,TCP_SEND_BUFF);
            memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
            memcpy(sendBuff + sizeof(QDB495_SendPackage),dataUnit.data.data(),dataUnit.data.length());

            int realSendLen = sendDataFunc(sendBuff,QDB495_SendPackage_Length + protocolDataLen + sliceLen);

            if(realSendLen == (QDB495_SendPackage_Length +protocolDataLen + sliceLen )){
                sendDataLen += sliceLen;
            }else{
                break;
            }
        }

        if(sendDataLen == originalData.length()){
            return true;
        }
        else
        {
            return false;
        }
    }
    //[~1]

    //文件数据，每一片文件数据均小于最大传输限制(1024byte)，不用继续分片
    if(dataUnit.cFileType == QDB2051::F_BINARY){
        int headLen = 0;
        if(dataUnit.usOrderNo == O_2051){
            int protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2051_Head_Length;
            headLen = protocolDataLen + dataUnit.cFilename.size();
        }

        int totalIndex = countTotoalIndex(dataUnit.dwPackAllLen);
        packet.dwPackAllLen = dataUnit.dwPackAllLen + totalIndex * headLen;
        packet.wPackLen = headLen + dataUnit.data.length();
        packet.wOffset = dataUnit.wOffset;
        RSingleton<TCP_WrapRule>::instance()->wrap(dataUnit);

        memset(sendBuff,0,TCP_SEND_BUFF);
        memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
        memcpy(sendBuff + sizeof(QDB495_SendPackage),dataUnit.data.data(),dataUnit.data.length());

        int realSendLen = sendDataFunc(sendBuff,packet.wPackLen);

        if(realSendLen == packet.wPackLen){
            return true;
        }
    }else if(dataUnit.cFileType == QDB2051::F_NO_SUFFIX || dataUnit.cFileType == QDB2051::F_TEXT){
        QByteArray originalData = dataUnit.data;
        //多个协议头长度
        int protocolDataLen = 0;
        if(dataUnit.usOrderNo == O_2051){
            protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2051_Head_Length;
        }else if(dataUnit.usOrderNo == 2048){
            protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2048_Head_Length;
        }
        int totalIndex = countTotoalIndex(originalData.length());
        packet.dwPackAllLen = originalData.length() + totalIndex * protocolDataLen;

        for(unsigned int i = 0; i < totalIndex; i++)
        {
            packet.wOffset = i;

            int leftDataLen = originalData.length() - sendDataLen;
            int sliceLen = leftDataLen > MAX_PACKET ? MAX_PACKET: leftDataLen;

            dataUnit.data.clear();
            dataUnit.data.append(originalData.mid(sendDataLen,sliceLen));
            RSingleton<TCP_WrapRule>::instance()->wrap(dataUnit);

            packet.wPackLen = protocolDataLen + sliceLen;

            memset(sendBuff,0,TCP_SEND_BUFF);
            memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
            memcpy(sendBuff + sizeof(QDB495_SendPackage),dataUnit.data.data(),dataUnit.data.length());

            int realSendLen = sendDataFunc(sendBuff,packet.wPackLen);

            if(realSendLen == packet.wPackLen){
                sendDataLen += (packet.wPackLen - protocolDataLen);
            }else{
                break;
            }
        }

        if(sendDataLen == originalData.length()){
            return true;
        }
    }

    return false;
}

bool TCP495DataPacketRule::unwrap(const char *data, const int length, DataHandler recvDataFunc)
{
    dhandler = recvDataFunc;
    bool unWrapResult = false;
    lastRecvBuff.clear();
    if(lastRecvBuff.size() > 0)
    {
        int tmpBuffLen = lastRecvBuff.size() + length + 1;
        char * dataBuff = new char[tmpBuffLen];
        memset(dataBuff,0,tmpBuffLen * sizeof(char));

        memcpy(dataBuff,lastRecvBuff.data(),lastRecvBuff.size());
        memcpy(dataBuff + lastRecvBuff.size(),data,length);

        lastRecvBuff.clear();
        unWrapResult = recvData(dataBuff,tmpBuffLen - 1);

        delete[] dataBuff;
    }
    else
    {
        unWrapResult = recvData(data,length);
    }

    return unWrapResult;
}

bool TCP495DataPacketRule::recvData(const char *recvData, int recvLen)
{
    QDB495_SendPackage packet;
    memset((char *)&packet,0,sizeof(QDB495_SendPackage));

    if(recvLen > sizeof(QDB495_SendPackage))
    {
        unsigned int processLen = 0;
        do
        {
            memcpy((char *)&packet,recvData+processLen,sizeof(QDB495_SendPackage));

            //TODO
            bool ret = true;
            unsigned short wSourceAddr = ntohs(packet.wSourceAddr);
            wSourceAddr = UnTransferAddr(wSourceAddr).toUShort();
            unsigned short wDestAddr = ntohs(packet.wDestAddr);
            wDestAddr = UnTransferAddr(wDestAddr).toUShort();

            packet.wSourceAddr = wSourceAddr;
            packet.wDestAddr = wDestAddr;
            packet.bPeserve = ntohs(packet.bPeserve);
            packet.wPackLen = ntohs(packet.wPackLen);
            packet.dwPackAllLen = ntohl(packet.dwPackAllLen);

            processLen += sizeof(QDB495_SendPackage);
            //[1]数据头部分正常
            if(true)
            {
                //[1.1]至少存在多余一个完整数据包
                int currentDataPackLen = packet.wPackLen;
                if(currentDataPackLen <= recvLen - processLen)
                {
                    //对数据包类型进行预判断处理
                    QDB21::QDB21_Head head21;
                    memset(&head21,0,sizeof(QDB21::QDB21_Head));
                    memcpy((char *)&head21,recvData + processLen,sizeof(QDB21::QDB21_Head));

                    //若协议为2051需要对文件的类型进行判断，若为2048则直接默认以text形式发送
                    int ptype = QDB2051::F_NO_SUFFIX;
                    if(head21.usOrderNo == O_2051){
                        QDB2051::QDB2051_Head head2051;
                        memset(&head2051,0,sizeof(QDB2051::QDB2051_Head));
                        memcpy((char *)&head2051,recvData + processLen + sizeof(QDB21::QDB21_Head),sizeof(QDB2051::QDB2051_Head));
                        ptype = (int)(head2051.cFileType);
                    }else if(head21.usOrderNo == O_2048){

                    }

                    if(ptype == QDB2051::F_TEXT || ptype == QDB2051::F_BINARY)
                    {
                        RecvUnit result;
                        result.extendData.method = C_TCP;
                        result.extendData.type495 = static_cast<PacketType_495>(packet.bPackType);
                        result.extendData.bPeserve = packet.bPeserve;
                        result.extendData.wOffset = packet.wOffset;
                        result.extendData.dwPackAllLen = packet.dwPackAllLen;
                        result.extendData.usSerialNo = packet.wSerialNo;

                        result.data.resize(currentDataPackLen);
                        memcpy(result.data.data(),recvData + processLen,currentDataPackLen);

                        if(result.data.size() > 0){
                            dhandler(result);
                        }
                    }
                    else if(ptype == QDB2051::F_NO_SUFFIX)
                    {
                        //[1.1.1]一包数据
                        if(packet.dwPackAllLen == packet.wPackLen)
                        {
                            RecvUnit result;
                            result.extendData.method = C_TCP;
                            result.extendData.type495 = static_cast<PacketType_495>(packet.bPackType);
                            result.extendData.bPeserve = packet.bPeserve;

                            result.data.resize(currentDataPackLen);
                            memcpy(result.data.data(),recvData + processLen,currentDataPackLen);

                            if(result.data.size() > 0){
                                dhandler(result);
                            }
                        }
                        //[1.1.2]多包数据(只保存数据部分)(默认协议为2051)
                        else
                        {
                           QByteArray data;
                           data.resize(currentDataPackLen);

                           //currentDataPackLen的长度=sizeof(21)+sizeof(2051)+真实数据长度
                           memcpy(data.data(),recvData + processLen,currentDataPackLen);

                           if(packetBuffs.value(packet.wSerialNo,NULL) == NULL)
                           {
                                PacketBuff * buff = new PacketBuff;
                                buff->totalPackLen = packet.dwPackAllLen;
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
                                    if(buff->recvSize == buff->totalPackLen)
                                    {
                                        buff->isCompleted = true;

                                        RecvUnit result;
                                        result.extendData.method = C_TCP;
                                        result.extendData.type495 = static_cast<PacketType_495>(packet.bPackType);
                                        result.extendData.bPeserve = packet.bPeserve;
                                        result.extendData.sliceNum = packet.wOffset + 1;

                                        int userPrtocolLen = sizeof(QDB21::QDB21_Head) + sizeof(QDB2051::QDB2051_Head);
                                        buff->packDataWidthPrtocol(result.data,userPrtocolLen);

                                        if(result.data.size() > 0){
                                            dhandler(result);
                                        }

                                        packetBuffs.remove(packet.wSerialNo);
                                        delete buff;
                                    }
                                }
                           }
                        }
                    }

                    processLen += currentDataPackLen;

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
                return false;
            }
        }while(processLen <= recvLen);
    }
    else
    {
        lastRecvBuff.clear();
        lastRecvBuff.append(recvData,recvLen);
    }
    return true;
}

/*!
 * @brief 计算数据总分包数量
 * @param[in] totalLength 数据总长度
 * @return 总分包大小
 */
int TCP495DataPacketRule::countTotoalIndex(const int totalLength)
{
    if(totalLength <= 0)
        return int(1);
    return qCeil(((float) totalLength/ MAX_PACKET));
}

}

#endif
