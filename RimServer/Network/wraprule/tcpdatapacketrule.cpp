#include "tcpdatapacketrule.h"
#include <qmath.h>

#include "../win32net/iocpcontext.h"
#include "../dataprocess/handler.h"

namespace ServerNetwork{

TCPDataPacketRule::TCPDataPacketRule():
    WrapRule(),handler(nullptr)
{
    SendPackId = qrand()%1024 + 1000;
}

/*!
 * @brief 封装发送数据，并传递发送到方式
 * @param[in] data 待发送的数据
 * @param[in] func 发送数据函数
 * @return 返回是否发送成功
 */
bool TCPDataPacketRule::wrap(const SendUnit &dataUnit, IocpContextSender sendFunc)
{
    TcpClient * client = TcpClientManager::instance()->getClient(dataUnit.sockId);
    if(client != NULL)
    {
        DataPacket packet;
        memset((char *)&packet,0,sizeof(DataPacket));
        packet.magicNum = SEND_MAGIC_NUM;

        packet.packId = client->getPackId();
        packet.totalIndex = qCeil(((float)dataUnit.dataUnit.data.length() / MAX_PACKET));
        packet.totalLen = dataUnit.dataUnit.data.size();

        int sendLen = 0;
        for(unsigned int i = 0; i < packet.totalIndex; i++)
        {
            packet.currentIndex = i;
            int leftLen = dataUnit.dataUnit.data.size() - sendLen;
            packet.currentLen = leftLen > MAX_PACKET ? MAX_PACKET: leftLen;

            int dataLen = packet.currentLen + sizeof(DataPacket);

            IocpContext * context = IocpContext::create(IocpType::IOCP_SEND,client);
            memcpy(context->getPakcet(),(char *)&packet,sizeof(DataPacket));
            memcpy(context->getPakcet()+ sizeof(DataPacket),dataUnit.dataUnit.data.data() + sendLen,packet.currentLen);

            context->getWSABUF().len = dataLen;

            DWORD realSendLen = 0;
            if(!sendFunc(dataUnit.sockId,context,realSendLen))
                return false;

            if(realSendLen == dataLen){
                sendLen += packet.currentLen;
            }
        }

        if(sendLen == packet.totalLen){
            return true;
        }
    }

    return false;
}

void TCPDataPacketRule::registDataHandler(Handler *dataHandler)
{
    handler = dataHandler;
}

void TCPDataPacketRule::bindContext(IocpContext * context, unsigned long recvLen)
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

void TCPDataPacketRule::wrap(ProtocolPackage &data)
{
    Q_UNUSED(data)
}

bool TCPDataPacketRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    Q_UNUSED(data)
    Q_UNUSED(result)
    return false;
}

void TCPDataPacketRule::recvData(const char * recvData,int recvLen)
{
    DataPacket packet;
    memset((char *)&packet,0,sizeof(DataPacket));

    if(recvLen >= sizeof(DataPacket))
    {
        int processLen = 0;
        do
        {
            memcpy((char *)&packet,recvData+processLen,sizeof(DataPacket));
            processLen += sizeof(DataPacket);
            //[1]数据头部分正常
            if(packet.magicNum == RECV_MAGIC_NUM)
            {
                RecvUnit socketData;
                socketData.extendData.sockId = ioContext->getClient()->socket();

                //[1.1]至少存在多余一个完整数据包
                if(packet.currentLen <= recvLen - processLen)
                {
                    //[1.1.1]一包数据
                    if(packet.totalIndex == 1)
                    {
                        socketData.data.resize(packet.currentLen);
                        memcpy(socketData.data.data(),recvData + processLen,packet.currentLen);

                        if(handler)
                            handler->handle(socketData);
                    }
                    //[1.1.2]多包数据(只保存数据部分)
                    else
                    {
                       QByteArray data;
                       data.resize(packet.currentLen);
                       memcpy(data.data(),ioContext->getPakcet() + processLen,packet.currentLen);

                       std::unique_lock<std::mutex> ul(ioContext->getClient()->BuffMutex());
                       if(ioContext->getClient()->getPacketBuffs().value(packet.packId,NULL) == NULL)
                       {
                            PacketBuff * buff = new PacketBuff;
                            buff->totalPackIndex = packet.totalIndex;
                            buff->recvPackIndex += 1;
                            buff->recvSize += packet.currentLen;
                            buff->buff.append(data);

                            ioContext->getClient()->getPacketBuffs().insert(packet.packId,buff);
                       }
                       else
                       {
                            PacketBuff * buff = ioContext->getClient()->getPacketBuffs().value(packet.packId,NULL);
                            if(buff)
                            {
                                buff->buff.append(data);
                                buff->recvSize += packet.currentLen;
                                buff->recvPackIndex += 1;
                                if(buff->recvPackIndex == buff->totalPackIndex)
                                {
                                    buff->isCompleted = true;

                                    socketData.data.append(buff->getFullData());

                                    ioContext->getClient()->getPacketBuffs().remove(packet.packId);
                                    delete buff;

                                    if(handler)
                                        handler->handle(socketData);
                                }
                            }
                       }
                    }

                    processLen += packet.currentLen;

                    //[1.1.3]检验是否满足下次处理需求
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
                    ioContext->getClient()->getHalfPacketBuff().append((char *)&packet,sizeof(DataPacket));
                    ioContext->getClient()->getHalfPacketBuff().append(recvData+processLen,leftLen);

                    processLen += leftLen;
                    break;
                }
            }
            else
            {
                 //TODO 20180718对错误处理
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


} // namespace ClientNetwork


