#include "tcp495datapacketrule.h"

#ifdef __LOCAL_CONTACT__

#include <qmath.h>
#include <QDebug>

#include "head.h"
#include "../win32net/iocpcontext.h"
#include "../dataprocess/handler.h"
#include "Util/rsingleton.h"
#include "tcp_wraprule.h"

namespace ServerNetwork{

using namespace QDB495;
using namespace QDB21;
using namespace QDB2051;
using namespace QDB2048;

TCP495DataPacketRule::TCP495DataPacketRule():
    WrapRule(),textHandler(nullptr)
{
    SendPackId = qrand()%1024;
}

void TCP495DataPacketRule::registTextHandler(Handler *dataHandler)
{
    textHandler = dataHandler;
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

/*!
 * @brief 使用iocp方式对原始数据添加协议，并且分片发送
 * @param[in] dunit 待发送给的数据单元
 * @param[in] sendFunc 发送回调函数
 * @return 是否发送成功
 */
bool TCP495DataPacketRule::wrap(SendUnit &dunit, IocpContextSender sendFunc)
{
    TcpClient * client = TcpClientManager::instance()->getClient(dunit.sockId);
    if(client != NULL)
    {
        QDB495_SendPackage packet;
        memset((char *)&packet,0,sizeof(QDB495_SendPackage));
        packet.bVersion = 1;
        packet.bPackType = dunit.dataUnit.bPackType;
        packet.bPriority = 0;
        packet.bPeserve = dunit.dataUnit.bPeserve;
        packet.wSerialNo = dunit.dataUnit.usSerialNo;
        packet.wCheckout = 0;
        packet.wDestAddr = dunit.dataUnit.wDestAddr;
        packet.wSourceAddr = dunit.dataUnit.wSourceAddr;

        int sendDataLen = 0;

        //文件数据，每一片文件数据均小于最大传输限制
        if(dunit.dataUnit.cFileType == QDB2051::F_BINARY){
            int headLen = 0;
            if(dunit.dataUnit.usOrderNo == O_2051){
                int protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2051_Head_Length;
                headLen = protocolDataLen + dunit.dataUnit.cFilename.size();
            }

            int totalIndex = countTotoalIndex(dunit.dataUnit.dwPackAllLen);
            packet.dwPackAllLen =dunit.dataUnit.dwPackAllLen + totalIndex * headLen;
            packet.wPackLen = headLen + dunit.dataUnit.data.length();
            packet.wOffset = dunit.dataUnit.wOffset;
            RSingleton<TCP_WrapRule>::instance()->wrap(dunit.dataUnit);

            IocpContext * context = IocpContext::create(IocpType::IOCP_SEND,client);
            memcpy(context->getPakcet(),(char *)&packet,sizeof(QDB495_SendPackage));
            memcpy(context->getPakcet()+ sizeof(QDB495_SendPackage),dunit.dataUnit.data.data(),dunit.dataUnit.data.length());

            context->getWSABUF().len = packet.wPackLen;

            DWORD realSendLen = 0;
            if(!sendFunc(dunit.sockId,context,realSendLen))
                return false;

            if(realSendLen == packet.wPackLen){
                return true;
            }
        }else if(dunit.dataUnit.cFileType == QDB2051::F_NO_SUFFIX || dunit.dataUnit.cFileType == QDB2051::F_TEXT){
            QByteArray originalData = dunit.dataUnit.data;

            //多个协议头长度
            int protocolDataLen = 0;
            if(dunit.dataUnit.usOrderNo == O_2051){
                protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2051_Head_Length;
            }else if(dunit.dataUnit.usOrderNo == O_2048){
                protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2048_Head_Length;
            }
            int totalIndex = countTotoalIndex(originalData.length());
            packet.dwPackAllLen = originalData.length() + totalIndex * protocolDataLen;

            for(unsigned int i = 0; i < totalIndex; i++)
            {
                packet.wOffset = i;

                int leftDataLen = originalData.length() - sendDataLen;
                int sliceLen = leftDataLen > MAX_PACKET ? MAX_PACKET: leftDataLen;

                dunit.dataUnit.data.clear();
                dunit.dataUnit.data.append(originalData.mid(sendDataLen,sliceLen));
                RSingleton<TCP_WrapRule>::instance()->wrap(dunit.dataUnit);

                packet.wPackLen = protocolDataLen + sliceLen;

                IocpContext * context = IocpContext::create(IocpType::IOCP_SEND,client);
                memcpy(context->getPakcet(),(char *)&packet,sizeof(QDB495_SendPackage));
                memcpy(context->getPakcet()+ sizeof(QDB495_SendPackage),dunit.dataUnit.data.data(),dunit.dataUnit.data.length());

                context->getWSABUF().len = packet.wPackLen;

                DWORD realSendLen = 0;
                if(!sendFunc(dunit.sockId,context,realSendLen))
                    return false;

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
    }

    return false;
}

bool TCP495DataPacketRule::wrap(SendUnit &dunit, ByteSender sendFunc)
{
    QDB495_SendPackage packet;
    memset((char *)&packet,0,sizeof(QDB495_SendPackage));
    packet.bVersion = 1;
    packet.bPackType = dunit.dataUnit.bPackType;
    packet.bPriority = 0;
    packet.bPeserve = dunit.dataUnit.bPeserve;
    packet.wSerialNo = dunit.dataUnit.usSerialNo;
    packet.wCheckout = 0;
    packet.wDestAddr = dunit.dataUnit.wDestAddr;
    packet.wSourceAddr = dunit.dataUnit.wSourceAddr;

    int sendDataLen = 0;

    //文件数据，每一片文件数据均小于最大传输限制
    if(dunit.dataUnit.cFileType == QDB2051::F_BINARY){
        int headLen = 0;
        if(dunit.dataUnit.usOrderNo == O_2051){
            int protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2051_Head_Length;
            headLen = protocolDataLen + dunit.dataUnit.cFilename.size();
        }

        int totalIndex = countTotoalIndex(dunit.dataUnit.dwPackAllLen);
        packet.dwPackAllLen = dunit.dataUnit.dwPackAllLen + totalIndex * headLen;
        packet.wPackLen = headLen + dunit.dataUnit.data.length();
        packet.wOffset = dunit.dataUnit.wOffset;
        RSingleton<TCP_WrapRule>::instance()->wrap(dunit.dataUnit);

        memset(sendBuff,0,TCP_SEND_BUFF);
        memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
        memcpy(sendBuff + sizeof(QDB495_SendPackage),dunit.dataUnit.data.data(),dunit.dataUnit.data.length());

        int realSendLen = sendFunc(dunit.sockId,sendBuff,packet.wPackLen);

        if(realSendLen == packet.wPackLen){
            return true;
        }
    }else if(dunit.dataUnit.cFileType == QDB2051::F_NO_SUFFIX || dunit.dataUnit.cFileType == QDB2051::F_TEXT){
        QByteArray originalData = dunit.dataUnit.data;

        //多个协议头长度
        int protocolDataLen = 0;
        if(dunit.dataUnit.usOrderNo == O_2051){
            protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2051_Head_Length;
        }else if(dunit.dataUnit.usOrderNo == O_2048){
            protocolDataLen = QDB495_SendPackage_Length + QDB21_Head_Length + QDB2048_Head_Length;
        }
        int totalIndex = countTotoalIndex(originalData.length());
        packet.dwPackAllLen = originalData.length() + totalIndex * protocolDataLen;

        for(unsigned int i = 0; i < totalIndex; i++)
        {
            packet.wOffset = i;

            int leftDataLen = originalData.length() - sendDataLen;
            int sliceLen = leftDataLen > MAX_PACKET ? MAX_PACKET: leftDataLen;

            dunit.dataUnit.data.clear();
            dunit.dataUnit.data.append(originalData.mid(sendDataLen,sliceLen));
            RSingleton<TCP_WrapRule>::instance()->wrap(dunit.dataUnit);

            packet.wPackLen = protocolDataLen + sliceLen;

            memset(sendBuff,0,TCP_SEND_BUFF);
            memcpy(sendBuff,(char *)&packet,sizeof(QDB495_SendPackage));
            memcpy(sendBuff + sizeof(QDB495_SendPackage),dunit.dataUnit.data.data(),dunit.dataUnit.data.length());

            int realSendLen = sendFunc(dunit.sockId,sendBuff,packet.wPackLen);

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

/*!
 * @brief 计算数据总分包数量
 * @param[in] totalLength 数据总长度
 * @return 总分包大小
 */
int TCP495DataPacketRule::countTotoalIndex(const int totalLength)
{
    return qCeil(((float) totalLength/ (MAX_PACKET)));
}

void TCP495DataPacketRule::wrap(ProtocolPackage &data)
{
    Q_UNUSED(data)
}

bool TCP495DataPacketRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    Q_UNUSED(data)
    Q_UNUSED(result)
    return false;
}

/*!
 * @brief 解析并处理数据，去除495传输控制协议(21、2051、2048等协议待应用层进行解析)
 * @note 根据495传输控制协议，将网络接收到的数据按照协议解析，并根据数据的类型分发处理; \n
 *       若2051协议中的fileType为0时，表示文本数据，则需要进行断包接收处理，并设立接收缓冲区; \n
 *       若2051协议中的fileType为1或2时，表示文件数据，则接收一包后直接交由应用层写入磁盘操作. \n
 * @param[in] recvData 接收到的网络数据
 * @param[in] recvLen 数据长度
 */
void TCP495DataPacketRule::recvData(const char *recvData, int recvLen)
{
    QDB495_SendPackage packet;
    memset((char *)&packet,0,sizeof(QDB495_SendPackage));

    if(recvLen >= sizeof(QDB495_SendPackage))
    {
        int processLen = 0;
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
                socketData.extendData.bPeserve = packet.bPeserve;
                socketData.extendData.wOffset = packet.wOffset;
                socketData.extendData.dwPackAllLen = packet.dwPackAllLen;

                //[1.1]至少存在多余一个完整数据包
                int currentDataPackLen = packet.wPackLen - QDB495_SendPackage_Length;
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
                        //2048中不包含相关协议字段，暂不解析
                    }

                    if(ptype == QDB2051::F_TEXT || ptype == QDB2051::F_BINARY)
                    {
                        socketData.data.resize(currentDataPackLen);
                        memcpy(socketData.data.data(),recvData + processLen,currentDataPackLen);
                        if(textHandler)
                            textHandler->handle(socketData);
                    }
                    else if(ptype == QDB2051::F_NO_SUFFIX)
                    {
                        //[1.1.1]一包数据
                        if(packet.dwPackAllLen == packet.wPackLen)
                        {
                            socketData.extendData.sliceNum = packet.wOffset + 1;
                            socketData.data.resize(currentDataPackLen);
                            memcpy(socketData.data.data(),recvData + processLen,currentDataPackLen);

                            if(textHandler)
                                textHandler->handle(socketData);
                        }
                        //[1.1.2]多包数据(只保存数据部分)(默认协议为2051)
                        else
                        {
                           QByteArray data;
                           data.resize(currentDataPackLen);

                           //currentDataPackLen的长度=sizeof(21)+sizeof(2051)+真实数据长度
                           memcpy(data.data(),ioContext->getPakcet() + processLen,currentDataPackLen);

                           std::unique_lock<std::mutex> ul(ioContext->getClient()->BuffMutex());
                           if(ioContext->getClient()->getPacketBuffs().value(packet.wSerialNo,NULL) == NULL)
                           {
                                PacketBuff * buff = new PacketBuff;
                                buff->totalPackLen = packet.dwPackAllLen;
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
                                    if(buff->recvSize == buff->totalPackLen)
                                    {
                                        buff->isCompleted = true;
                                        socketData.extendData.sliceNum = packet.wOffset + 1;

                                        int userPrtocolLen = sizeof(QDB21::QDB21_Head) + sizeof(QDB2051::QDB2051_Head);
                                        buff->packDataWidthPrtocol(socketData.data,userPrtocolLen);

                                        ioContext->getClient()->getPacketBuffs().remove(packet.wSerialNo);
                                        delete buff;

                                        if(textHandler)
                                            textHandler->handle(socketData);
                                    }
                                }
                           }
                        }
                    }

                    processLen += currentDataPackLen;

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

}

#endif
