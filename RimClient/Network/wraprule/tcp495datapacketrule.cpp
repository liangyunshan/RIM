#include "tcp495datapacketrule.h"

#ifdef __LOCAL_CONTACT__

#include <qmath.h>
#include <QDebug>

#include "Network/network_global.h"
#include "Util/rsingleton.h"
#include "tcp_wraprule.h"

namespace ClientNetwork{

using namespace QDB495;
using namespace QDB21;
using namespace QDB2051;
using namespace QDB2048;

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

/*!
 * @brief 将待发送的原始数据加上数据传输协议头
 * @note 原始的数据根据协议的类型，加上不同的传输协议头： @n
 *       文本信息：【495】【21】【2051】【Data】 @n
 *       文本确认信息：【495】【21】【2048】 @n
 *       文件信息：【495】【21】【2051】 @n
 *
 *       @date 20180730
 *             1.若因data过长导致数据分包，那么只有在第一包中以【495】【21】【2051】【文件名】【Data】协议组成，其它包均为【495】【Data】组成。 @n
 *             2.495中单包数据长度以及总数据长度，均不包括495本身的数据长度。
 *
 * @param[in] dataUnit 待发送数据描述信息
 * @param[in] sendDataFunc 数据发送函数
 * @return 是否发送成功
 */
bool TCP495DataPacketRule::wrap(ProtocolPackage &dataUnit, std::function<int (const char *, const int)> sendDataFunc)
{
    QDB495_SendPackage packet;
    memset((char *)&packet,0,sizeof(QDB495_SendPackage));
    packet.bVersion = 1;
    packet.bPackType = dataUnit.bPackType;
    packet.bPriority = 0;
    packet.bPeserve = dataUnit.bPeserve;
    packet.wSerialNo = dataUnit.usSerialNo;
    packet.wCheckout = 0;
    packet.wDestAddr = dataUnit.wDestAddr;
    packet.wSourceAddr = dataUnit.wSourceAddr;

    int sendDataLen = 0;

    //文件数据，每一片文件数据均小于最大传输限制(512byte)，不用继续分片
    if(dataUnit.cFileType == QDB2051::F_BINARY)
    {
        int protocolDataLen = 0;
        if(dataUnit.wOffset == 0)
        {
            if(dataUnit.usOrderNo == O_2051){
                protocolDataLen = QDB21_Head_Length + QDB2051_Head_Length + dataUnit.cFilename.length();
            }
            RSingleton<TCP_WrapRule>::instance()->wrap(dataUnit);
            packet.wPackLen = dataUnit.data.length();
        }
        else
        {
            packet.wPackLen = dataUnit.data.length();
        }

        packet.dwPackAllLen = dataUnit.dwPackAllLen + protocolDataLen ;
        packet.wOffset = dataUnit.wOffset;

        unsigned short currPackTotalLen = packet.wPackLen + sizeof(QDB495_SendPackage);

        memset(sendBuff,0,TCP_SEND_BUFF);
        memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
        memcpy(sendBuff + sizeof(QDB495_SendPackage),dataUnit.data.data(),dataUnit.data.length());

        int realSendLen = sendDataFunc(sendBuff,currPackTotalLen);

        if(realSendLen == currPackTotalLen){
            return true;
        }
    }
    //文本数据
    else if(dataUnit.cFileType == QDB2051::F_NO_SUFFIX || dataUnit.cFileType == QDB2051::F_TEXT)
    {
        //多个协议头长度
        int protocolDataLen = 0;
        switch(dataUnit.usOrderNo){
           case O_NONE:
                protocolDataLen = 0;
                break;
           case O_2051:
                protocolDataLen =  QDB21_Head_Length + QDB2051_Head_Length;
                break;
           case O_2048:
                protocolDataLen =  QDB21_Head_Length + QDB2048_Head_Length;
                break;
           default:
                break;
        }

        packet.dwPackAllLen = dataUnit.data.length() + protocolDataLen;
        int totalIndex = countTotoalIndex(packet.dwPackAllLen);

        //添加21、2051协议头
        if(dataUnit.usOrderNo != O_NONE)
            RSingleton<TCP_WrapRule>::instance()->wrap(dataUnit);

        QByteArray originalData = dataUnit.data;

        for(unsigned int i = 0; i < totalIndex; i++)
        {
            packet.wOffset = i;

            int leftDataLen = packet.dwPackAllLen - sendDataLen;
            int sliceLen = leftDataLen > MAX_PACKET ? MAX_PACKET: leftDataLen;

            dataUnit.data.clear();
            dataUnit.data.append(originalData.mid(sendDataLen,sliceLen));

            packet.wPackLen = sliceLen;
            unsigned short currPackTotalLen = packet.wPackLen + sizeof(QDB495_SendPackage);

            memset(sendBuff,0,TCP_SEND_BUFF);
            memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
            memcpy(sendBuff + sizeof(QDB495_SendPackage),dataUnit.data.data(),dataUnit.data.length());

            int realSendLen = sendDataFunc(sendBuff,currPackTotalLen);
            if(realSendLen == currPackTotalLen){
                sendDataLen += packet.wPackLen;
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
            processLen += sizeof(QDB495_SendPackage);
            //[1]数据头部分正常
            if(true)
            {
                //[1.1]至少存在多余一个完整数据包
                int currentDataPackLen = packet.wPackLen;
                if(currentDataPackLen <= recvLen - processLen)
                {
                    //若协议为2051需要对文件的类型进行判断，若为2048则直接默认以text形式发送
                    FileType ptype = QDB2051::F_NO_SUFFIX;

                    if(packet.wOffset == 0)
                    {
                        //对数据包类型进行预判断处理
                        QDB21::QDB21_Head head21;
                        memset(&head21,0,sizeof(QDB21::QDB21_Head));
                        memcpy((char *)&head21,recvData + processLen,sizeof(QDB21::QDB21_Head));

                        if(head21.usOrderNo == O_2051){
                            QDB2051::QDB2051_Head head2051;
                            memset(&head2051,0,sizeof(QDB2051::QDB2051_Head));
                            memcpy((char *)&head2051,recvData + processLen + sizeof(QDB21::QDB21_Head),sizeof(QDB2051::QDB2051_Head));
                            ptype = static_cast<FileType>((head2051.cFileType));
                        }else if(head21.usOrderNo == O_2048){

                        }
                    }
                    else
                    {
                        if(queryFiletype(RecvFileTypeId(packet.wSourceAddr,packet.wDestAddr,packet.wSerialNo),ptype)){

                        }
                    }

                    RecvUnit result;
                    result.extendData.method = C_TCP;
                    result.extendData.fileType = ptype;
                    result.extendData.type495 = static_cast<PacketType_495>(packet.bPackType);
                    result.extendData.bPeserve = packet.bPeserve;
                    result.extendData.wOffset = packet.wOffset;
                    result.extendData.dwPackAllLen = packet.dwPackAllLen;
                    result.extendData.usSerialNo = packet.wSerialNo;
                    result.extendData.wDestAddr = packet.wDestAddr;
                    result.extendData.wSourceAddr = packet.wSourceAddr;

                    if(ptype == QDB2051::F_TEXT || ptype == QDB2051::F_BINARY)
                    {
                        if(packet.wOffset == 0){
                           //TODO 20180731考虑如何删除指定的记录
                           addFileId(RecvFileTypeId(RecvFileTypeId(packet.wSourceAddr,packet.wDestAddr,packet.wSerialNo,ptype)));
                        }
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
                                addFileId(RecvFileTypeId(RecvFileTypeId(packet.wSourceAddr,packet.wDestAddr,packet.wSerialNo,ptype)));
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
                                        result.extendData.sliceNum = packet.wOffset + 1;

                                        int userPrtocolLen = sizeof(QDB21::QDB21_Head) + sizeof(QDB2051::QDB2051_Head);
                                        buff->packDataWidthPrtocol(result.data,userPrtocolLen);

                                        packetBuffs.remove(packet.wSerialNo);
                                        delete buff;

                                        removeFileId(RecvFileTypeId(RecvFileTypeId(packet.wSourceAddr,packet.wDestAddr,packet.wSerialNo,ptype)));

                                        if(result.data.size() > 0){
                                            dhandler(result);
                                        }
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

bool TCP495DataPacketRule::addFileId(RecvFileTypeId fileId)
{
    std::lock_guard<std::recursive_mutex> lg(fileTypeMutex);

    QDB2051::FileType pType;
    if(queryFiletype(fileId,pType))
        return true;

    fileTypeVecs.push_back(fileId);

    return true;
}

void TCP495DataPacketRule::removeFileId(RecvFileTypeId fileId)
{
    std::lock_guard<std::recursive_mutex> lg(fileTypeMutex);

    auto beg = fileTypeVecs.begin();
    while(beg != fileTypeVecs.end()){
        if(*beg == fileId){
           beg = fileTypeVecs.erase(beg);
           continue;
        }
        beg++;
    }
}

bool TCP495DataPacketRule::queryFiletype(RecvFileTypeId &fileId, FileType &pType)
{
    std::lock_guard<std::recursive_mutex> lg(fileTypeMutex);
    auto findex = std::find_if(fileTypeVecs.begin(),fileTypeVecs.end(),[&](RecvFileTypeId & existFileId){
        return existFileId == fileId;
    });

    if(findex == fileTypeVecs.end())
        return false;

    pType = (*findex).filetype;

    return true;
}

}

#endif
