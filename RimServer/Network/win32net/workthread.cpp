#include "workthread.h"

#include <QDebug>
#include <Mswsock.h>
#include <process.h>

#include "tcpclient.h"
#include "netutils.h"
#include "Util/rlog.h"
#include "netglobal.h"

namespace ServerNetwork {

WorkThread::WorkThread(SharedIocpData *data):
    serverSharedData(data)
{
    threadId = (HANDLE)_beginthreadex(nullptr, 0,iocpProc,this, 0, nullptr);
}

unsigned  __stdcall iocpProc(LPVOID v)
{
    WorkThread * thread = (WorkThread *)v;

    unsigned long recvLength;
    TcpClient * recvClient;

    IocpContext * ioData;

    while (true)
    {
        recvLength = 0;
        ioData = NULL;

        int ret = GetQueuedCompletionStatus(thread->serverSharedData->m_ioCompletionPort, &recvLength, (PULONG_PTR)&recvClient, (LPOVERLAPPED*)&ioData, WSA_INFINITE);
        if(ret == 0)
        {
            RLOG_ERROR("Get iocp error!");
            thread->handleClose(ioData);
            continue;
        }

        if(recvLength == 0 && ioData->getType() == IOCP_RECV)
        {
            thread->serverSharedData->m_clientManager->remove(recvClient);
            continue;
        }

        thread->handleIo(ioData,recvLength,recvClient);
    }

    return 0;
}

void WorkThread::handleIo(IocpContext *ioData, unsigned long recvLength, TcpClient *recvClient)
{
    Q_ASSERT(ioData != nullptr);

    switch (ioData->getType())
    {
        case IOCP_RECV:
                        {
                            handleRecv(ioData,recvLength,recvClient);
                        }
                        break;
        case IOCP_ACCPET:
                        {
                            handleAccept(ioData);
                        }
                        break;
        case IOCP_SEND:
                        {
                            handleSend(ioData);
                        }
                        break;
        default:break;
    }
}

void WorkThread::handleRecv(IocpContext *ioData, unsigned long recvLen, TcpClient* tcpClient)
{
    ioData->getPakcet()[recvLen] = 0;

    int lastRecvBuffSize = ioData->getClient()->getHalfPacketBuff().size();

    if(lastRecvBuffSize > 0)
    {
        int tmpBuffLen = lastRecvBuffSize + recvLen + 1;
        char * dataBuff = new char[tmpBuffLen];
        memset(dataBuff,0,tmpBuffLen * sizeof(char));

        memcpy(dataBuff,ioData->getClient()->getHalfPacketBuff().data(),lastRecvBuffSize);
        memcpy(dataBuff + lastRecvBuffSize,ioData->getPakcet(),recvLen);

        ioData->getClient()->getHalfPacketBuff().clear();
        processRecvData(dataBuff,lastRecvBuffSize + recvLen,ioData);

        delete[] dataBuff;
    }
    else
    {
        processRecvData(ioData->getPakcet(),recvLen,ioData);
    }

    DWORD dwRecv = 0;
    DWORD dwFlags = 0;

    WSARecv(tcpClient->socket(),&(ioData->getWSABUF()), 1, &dwRecv, &dwFlags,&(ioData->getOverLapped()), NULL);
}

void WorkThread::processRecvData(char * recvData,unsigned long recvLen,IocpContext *ioData)
{
    DataPacket packet;
    memset((char *)&packet,0,sizeof(DataPacket));

    if(recvLen >= sizeof(DataPacket))
    {
        unsigned int processLen = 0;
        do
        {
            memcpy((char *)&packet,recvData+processLen,sizeof(DataPacket));
            processLen += sizeof(DataPacket);
            //[1]数据头部分正常
            if(packet.magicNum == RECV_MAGIC_NUM)
            {
                SocketInData socketData;
                socketData.sockId = ioData->getClient()->socket();

                //[1.1]至少存在多余一个完整数据包
                if(packet.currentLen <= recvLen - processLen)
                {
                    //[1.1.1]一包数据
                    if(packet.totalIndex == 1)
                    {
                        socketData.data.resize(packet.currentLen);
                        memcpy(socketData.data.data(),recvData + processLen,packet.currentLen);

                        G_RecvMutex.lock();
                        G_RecvButts.enqueue(socketData);
                        G_RecvMutex.unlock();

                        G_RecvCondition.wakeOne();
                    }
                    //[1.1.2]多包数据(只保存数据部分)
                    else
                    {
                       QByteArray data;
                       data.resize(packet.currentLen);
                       memcpy(data.data(),ioData->getPakcet() + processLen,packet.currentLen);

                       ioData->getClient()->lock();
                       if(ioData->getClient()->getPacketBuffs().value(packet.packId,NULL) == NULL)
                       {
                            PacketBuff * buff = new PacketBuff;
                            buff->totalPackIndex = packet.totalIndex;
                            buff->recvPackIndex += 1;
                            buff->recvSize += packet.currentLen;
                            buff->buff.append(data);

                            ioData->getClient()->getPacketBuffs().insert(packet.packId,buff);
                       }
                       else
                       {
                            PacketBuff * buff = ioData->getClient()->getPacketBuffs().value(packet.packId,NULL);
                            if(buff)
                            {
                                buff->buff.append(data);
                                buff->recvSize += packet.currentLen;
                                buff->recvPackIndex += 1;
                                if(buff->recvPackIndex == buff->totalPackIndex)
                                {
                                    buff->isCompleted = true;

                                    socketData.data.append(buff->getFullData());

                                    ioData->getClient()->getPacketBuffs().remove(packet.packId);
                                    delete buff;

                                    G_RecvMutex.lock();
                                    G_RecvButts.enqueue(socketData);
                                    G_RecvMutex.unlock();

                                    G_RecvCondition.wakeOne();
                                }
                            }
                       }
                       ioData->getClient()->unLock();
                    }

                    processLen += packet.currentLen;

                    //[1.1.3]检验是否满足下次处理需求
                    int leftLen = recvLen - processLen;
                    if(leftLen <= 0)
                    {
//                        qDebug()<<"[1]++++"<<leftLen<<"_"<<recvLen<<"_"<<processLen<<"_"<<packet.currentLen;
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

                        ioData->getClient()->lock();
                        ioData->getClient()->getHalfPacketBuff().clear();
                        ioData->getClient()->getHalfPacketBuff().append(recvData + processLen,leftLen);
                        ioData->getClient()->unLock();
                        processLen += leftLen;
                        break;
                    }
//                    qDebug()<<"[3]"<<processLen<<"__"<<recvLen;
                }
                //[1.2]【信息被截断】
                else
                {
                    int leftLen = recvLen - processLen;

                    ioData->getClient()->lock();
                    ioData->getClient()->getHalfPacketBuff().clear();
                    ioData->getClient()->getHalfPacketBuff().append((char *)&packet,sizeof(DataPacket));
                    ioData->getClient()->getHalfPacketBuff().append(recvData+processLen,leftLen);
                    ioData->getClient()->unLock();

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
        ioData->getClient()->lock();
        ioData->getClient()->getHalfPacketBuff().clear();
        ioData->getClient()->getHalfPacketBuff().append((char *)recvData,recvLen);
        ioData->getClient()->unLock();
    }
}

void WorkThread::handleAccept(IocpContext *ioData)
{
    int addrSize = (sizeof(struct sockaddr_in) + 16);

    struct sockaddr_in* lockAddr = NULL;
    int lockLen = sizeof(struct sockaddr_in);

    struct sockaddr_in* remoteAddr = NULL;
    int remoteLen = sizeof(struct sockaddr_in);

    //获取连接socket的信息
    GetAcceptExSockaddrs(ioData->getWSABUF().buf,0,addrSize, addrSize,
            (struct sockaddr**)&lockAddr, &lockLen,
                (struct sockaddr**)&remoteAddr, &remoteLen);

    serverSharedData->m_clientManager->addClient(ioData->getClient());

    NetUtil::crateIocp(ioData->getClient()->socket(), serverSharedData, (DWORD)ioData->getClient());

    NetUtil::postRecv(ioData);

    NetUtil::postAccept(serverSharedData);
}

void WorkThread::handleSend(IocpContext *ioData)
{
    if(ioData->getClient() != NULL)
    {
//        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"delete:"<<ioData->getClient()->socket();
        IocpContext::destory(ioData);
    }
}

void WorkThread::handleClose(IocpContext *ioData)
{
    if(ioData->getClient() != NULL)
    {
        qDebug()<<"11111111___delete client";
        serverSharedData->m_clientManager->remove(ioData->getClient());
        IocpContext::destory(ioData);
    }
}

}  //namespace ServerNetwork
