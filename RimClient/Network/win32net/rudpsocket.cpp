#include "rudpsocket.h"
#include <QDateTime>
#include <QTime>
#include <QTimer>
#include <QUdpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDataStream>

const QTime g_2000Time(0,0,0);
const int g_PortDiff = 1;

RUDPSocket::RUDPSocket(QHostAddress host, int localRecvPort, QObject *parent)
    : m_RecvPort(localRecvPort),QObject(parent)
{
    int ret1,ret2 ;
    m_udpSendScocket = new QUdpSocket();
    ret1 = m_udpSendScocket->bind(m_RecvPort+g_PortDiff);

    m_udpRecvScocket = new QUdpSocket();
    ret2 = m_udpRecvScocket->bind(QHostAddress::AnyIPv4, m_RecvPort);
    connect(m_udpRecvScocket, SIGNAL(readyRead()),
            this, SLOT(readPendingDatagrams()));

    m_ResendTimer = new QTimer(parent);
    m_ResendTimer->setInterval(1*1000);//1s
    connect(m_ResendTimer,SIGNAL(timeout()),
            this,SLOT(slot_ResendTimerOut()));

    m_CheckValidityTimer = new QTimer(parent);
    m_CheckValidityTimer->setInterval(2*1000);//3s
    connect(m_CheckValidityTimer,SIGNAL(timeout()),
            this,SLOT(slot_CheckValidityTimerOut()));

    m_lastRecvPacketBuff = NULL;
    inSendWork = false;
}

RUDPSocket::~RUDPSocket()
{
    if(m_udpSendScocket)
    {
        delete m_udpSendScocket;
        m_udpSendScocket = NULL;
    }

    if(m_udpRecvScocket)
    {
        disconnect(m_udpRecvScocket, SIGNAL(readyRead()),
                this, SLOT(readPendingDatagrams()));
        delete m_udpRecvScocket;
        m_udpRecvScocket = NULL;
    }

    if(m_ResendTimer)
    {
        disconnect(m_ResendTimer,SIGNAL(timeout()),
                this,SLOT(slot_ResendTimerOut()));
        delete m_ResendTimer;
        m_ResendTimer = NULL;
    }

    if(m_CheckValidityTimer)
    {
        disconnect(m_CheckValidityTimer,SIGNAL(timeout()),
                this,SLOT(slot_NckTimerOut()));
        delete m_CheckValidityTimer;
        m_CheckValidityTimer = NULL;
    }
}

//发送可靠传输数据
qint64 RUDPSocket::SendData(bool isFile, const char *pData, int iSize, QHostAddress &host, quint16 port)
{
    //判断是否需要分组发送
    if(iSize <= MAX_PACKAGE_SIZE)
    {
        SendBuff_UDP * waitbuff = new SendBuff_UDP;
        waitbuff->totalPackIndex = 1;
        waitbuff->sendSize += iSize;
        waitbuff->sendedPackNums +=1;
        waitbuff->lastSendedIndex =1;
        waitbuff->usPackageNo = QTime::currentTime().msecsTo(g_2000Time) % 65535;//65535为 us的表示范围
        waitbuff->addr = host;
        waitbuff->port = port;
        waitbuff->isFile = isFile;
        waitbuff->buff.insert(1,QByteArray(pData,iSize));
        AddSendBuffToQueue(waitbuff);
    }
    else
    {
        SendBuff_UDP * waitbuff = SplitePackageCount(pData,iSize);
        waitbuff->sendSize += iSize;
        waitbuff->isFile = isFile;
        waitbuff->addr = host;
        waitbuff->port = port;
        waitbuff->usSendedNums +=1;
        waitbuff->lastSendedIndex =1;
        waitbuff->usPackageNo = QTime::currentTime().msecsTo(g_2000Time) % 65535;//65535为 us的表示范围
        AddSendBuffToQueue(waitbuff);
    }

    int sendSize = slot_RestartSend();
    return sendSize;
}

qint64 RUDPSocket::SendData(SendBuff_UDP *buff)
{
    int sendSize = -1;
    if(buff)
    {
        if(buff && buff->sendSize <= MAX_PACKAGE_SIZE)
        {
            sendSize = SendNotSwitchData(buff);
        }
        else
        {
            sendSize = SendSwitchHeadData(buff);
        }
    }
    return sendSize;
}

//发送文件
qint64 RUDPSocket::SendFile(QString filename, QHostAddress &host, quint16 port)
{
    QByteArray data;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return -1;

    while (!file.atEnd()) {
      QByteArray line = file.readLine();
      data+=line;
    }
    return this->SendData(1,data.data(),data.size(),host,port);
}

//发送不可靠数据
qint64 RUDPSocket::SendQtData(const char *pData, int iSize, QHostAddress &host, quint16 port)
{
    qint64 sendedSize = m_udpSendScocket->writeDatagram(pData,iSize,host,port);
    return sendedSize;
}

//获取获取的值
qint64 RUDPSocket::readDatagram(char *data, qint64 *maxlen, QHostAddress *host, quint16 *port)
{
    int size = -1;
    if(m_lastRecvPacketBuff)
    {
        memcpy(data,m_lastRecvPacketBuff->getFullData(),m_lastRecvPacketBuff->getFullData().size());
        *maxlen = m_lastRecvPacketBuff->getFullData().size();
        *host = m_lastRecvPacketBuff->addr;
        *port = m_lastRecvPacketBuff->port;
        size = *maxlen;
    }

    return size;
}

QByteArray RUDPSocket::readLastData()
{
    return m_LastRecvData;
}

//重发计时器
void RUDPSocket::slot_ResendTimerOut()
{
    SendBuff_UDP *buff = (SendBuff_UDP*)m_WaitSendQueue.head();
    if(!buff)
    {
        m_WaitSendQueue.clear();
        StopResendTimer();
        return ;
    }
    if(buff->usSendedNums>=2)
    {
        //清空
        delete buff;
        buff =NULL;

        StopResendTimer();
        m_WaitSendQueue.dequeue();
        return ;
    }
    if(buff->totalPackIndex == 1)
    {
        if(buff->usSendedNums>=2)
        {
            StopResendTimer();
            m_WaitSendQueue.dequeue();
            return ;
        }
        else
        {
            buff->usSendedNums+=1;
            SendNotSwitchData(buff);
        }
    }
    else if(buff->totalPackIndex > 1)
    {
        if(buff->lastSendedIndex == 1)
        {
            buff->usSendedNums+=1;
            SendSwitchHeadData(buff);
        }
        else
        {
            buff->usSendedNums+=1;
            SendSwitchFllowData(buff->isFile,
                                buff->usPackageNo,
                                buff->buff.value(buff->lastSendedIndex).data(),
                                buff->buff.value(buff->lastSendedIndex).size(),
                                buff->lastSendedIndex,
                                buff->addr,
                                buff->port);
        }
    }
    else
    {
        StopResendTimer();
        m_WaitSendQueue.dequeue();
    }
}

//在一定的时间内收不到回应,则认为已经断线，我们把发送的队列清空
void RUDPSocket::slot_CheckValidityTimerOut()
{
    m_WaitSendQueue.clear();
    while(!m_WaitSendQueue.isEmpty())
    {
        SendBuff_UDP * buff = (SendBuff_UDP*) m_WaitSendQueue.dequeue();
        delete buff;
        buff = NULL;
    }
    inSendWork = false;
    m_WaitSendQueue.clear();
    StopCheckValidityTimer();
}

//接收原始的UDP数据
void RUDPSocket::readPendingDatagrams()
{
    int BytesReceived = 0;
    while (m_udpRecvScocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        BytesReceived = m_udpRecvScocket->pendingDatagramSize();
        datagram.resize(BytesReceived);
        QHostAddress sender;
        quint16 senderPort;

        qint64 readsize = m_udpRecvScocket->readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        GJB4908_UDP_NotSwitchGroup  header4908;
        memset(&header4908,0,sizeof(GJB4908_UDP_NotSwitchGroup));
        memcpy(&header4908,datagram.data(),sizeof(GJB4908_UDP_NotSwitchGroup));

        QByteArray userPackage;
        userPackage.clear();
        int userPackage_Size = 0;
        char *tmp = NULL;

        unsigned char ucFlagAndType = header4908.transHeader.ucFlagAndType;

        switch(ucFlagAndType & 0XF0)//网络报文类型
        {
        case 0X10://不分组报文
            if(header4908.transHeader.ucReserve == RAW)
            {
                QByteArray userPackage;
                userPackage.clear();
                userPackage_Size = datagram.size()-sizeof(GJB4908_UDP_NotSwitchGroup);
                tmp = new char[userPackage_Size];
                memcpy(tmp,datagram.data()+sizeof(GJB4908_UDP_NotSwitchGroup),userPackage_Size);
                userPackage.append(tmp,userPackage_Size);

                //收到是原始数据，需要对信息来源台位回复确认收到
                AckNotSwitchGroup(header4908,sender, senderPort-g_PortDiff);

                PacketBuff_UDP * buff = new PacketBuff_UDP;
                buff->isCompleted = true;
                buff->totalPackIndex = 1;
                buff->packageNo = header4908.transHeader.usPackageNo;
                buff->recvSize += header4908.transHeader.usPackageLen-sizeof(GJB4908_UDP_NotSwitchGroup);
                buff->buff.insert(1,userPackage);
                m_RecvQueue.append(buff);
                FetckQueueData(buff->packageNo);

                if(header4908.messageArea.ucMessMarking & 0X10 == 0x10)
                {
                    //存储接收到的信息为本地file
                    static QTime InitTime = QTime::currentTime();
                    QTime currTime = QTime::currentTime();
                    int diffTime =  currTime.msecsTo(InitTime);
                    qsrand(diffTime);
                    int randNum = qrand();
                    QString filename = QString("%1-%2").arg(randNum);
                    QFile file(filename);
                    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                        return;

                    QDataStream out(&file);
                    out << userPackage;
                    emit readyReadFile(filename);
                }
                else
                {
                    emit readyRead();
                }
            }
            else if(header4908.transHeader.ucReserve == ACK)
            {
                foreach(SendBuff_UDP *buff,m_WaitSendQueue)
                {
                    if(buff)
                    {
                        if(buff->usPackageNo == header4908.transHeader.usPackageNo)
                        {
                            m_WaitSendQueue.removeOne(buff);
                            delete buff;
                            buff = NULL;

                            StopResendTimer();
                            inSendWork = false;
                            slot_RestartSend();

                            break;
                        }
                    }
                }

            }
            break;//[~case 0X10]

        case 0X20://分组首报文
            GJB4908_UDP_HeadGroup HeadGroup;
            memset(&HeadGroup,0,sizeof(GJB4908_UDP_HeadGroup));
            memcpy(&HeadGroup,datagram.data(),sizeof(GJB4908_UDP_HeadGroup));

            userPackage.clear();
            userPackage_Size = datagram.size()-sizeof(GJB4908_UDP_HeadGroup);
            tmp = new char[userPackage_Size];
            memcpy(tmp,datagram.data()+sizeof(GJB4908_UDP_HeadGroup),userPackage_Size);
            userPackage+=QByteArray(tmp,userPackage_Size);

            if(HeadGroup.transHeader.ucReserve == RAW)
            {
                PacketBuff_UDP * buff = NULL;
                bool isExited = false;
                foreach(buff,m_RecvQueue)
                {
                    if(buff && buff->packageNo == HeadGroup.transHeader.usPackageNo)
                    {
                        isExited = true;
                        break;
                    }
                }
                if(!isExited)
                {
                    buff = new PacketBuff_UDP;
                    buff->addr = sender;
                    buff->port = senderPort;
                    buff->totalPackIndex = HeadGroup.ucPackageTotleNum;
                    buff->packageNo = HeadGroup.transHeader.usPackageNo;
                    buff->recvSize += HeadGroup.transHeader.usPackageLen-sizeof(GJB4908_UDP_HeadGroup);
                    buff->buff.insert(1,userPackage);
                    m_RecvQueue.enqueue(buff);

                    //收到是原始数据，需要对信息来源台位回复确认收到
                    AckHeadGroup(HeadGroup,sender, senderPort-1);
                    StartCheckValidityTimer();
                }
            }
            else if(HeadGroup.transHeader.ucReserve == ACK)
            {
                StopResendTimer();
                SendBuff_UDP * buff = NULL;

                foreach(buff,m_WaitSendQueue)
                {
                    if(buff->usPackageNo == HeadGroup.transHeader.usPackageNo)
                    {
                        if(buff->buff.contains(1))
                        {
                            buff->buff.remove(1);
                        }
                        //需要触发-发送等待发送的续报文
                        SendSwitchFllowData(buff);
                        buff->usSendedNums = 1;
                        buff->lastSendedIndex = 2;

                        break;
                    }
                }
            }
            break;//[~case 0X20]

        case 0X30://分组续报文
            GJB4908_UDP_FollowGroup FollowGroup;
            memset(&FollowGroup,0,sizeof(GJB4908_UDP_FollowGroup));
            memcpy(&FollowGroup,datagram.data(),sizeof(GJB4908_UDP_FollowGroup));

            userPackage.clear();
            userPackage_Size = datagram.size()-sizeof(GJB4908_UDP_FollowGroup);
            tmp = new char[userPackage_Size];
            memcpy(tmp,datagram.data()+sizeof(GJB4908_UDP_FollowGroup),userPackage_Size);
            userPackage.append(tmp,userPackage_Size);

            static int numssss = 0;
            if(FollowGroup.transHeader.ucReserve == RAW)
            {
                StartCheckValidityTimer();
                PacketBuff_UDP * buff = NULL;
                bool isExited = false;
                foreach(buff,m_RecvQueue)
                {
                    if(buff && buff->packageNo == FollowGroup.transHeader.usPackageNo)
                    {
                        isExited = true;
                        break;
                    }
                }
                if(isExited)
                {
                    //先判断这个序号是否已经接收过了
                    if(buff->buff.contains(FollowGroup.ucPackageSwitchNum))
                    {
                        //丢弃
                        return ;
                    }
                    //添加数据到接收缓存
                    buff->recvSize += FollowGroup.transHeader.usPackageLen-sizeof(GJB4908_UDP_FollowGroup);
                    buff->buff.insert(FollowGroup.ucPackageSwitchNum,userPackage);

                    if(buff->buff.count() == buff->totalPackIndex)
                    {
                        StopCheckValidityTimer();
                        buff->isCompleted = true;
                        //对数据进行排序重组
                        buff->Sort();
                        FetckQueueData(buff->packageNo);
                        emit readyRead();
                    }
                }
                //收到是原始数据，需要对信息来源台位回复确认收到
                AckFollowGroup(FollowGroup,sender, senderPort-1);

            }
            else if(FollowGroup.transHeader.ucReserve == ACK)
            {
                SendBuff_UDP * buff = NULL;
                foreach(buff,m_WaitSendQueue)
                {
                    if(buff->usPackageNo == FollowGroup.transHeader.usPackageNo)
                    {
                        if(buff->buff.contains(FollowGroup.ucPackageSwitchNum))
                        {
                            buff->buff.remove(FollowGroup.ucPackageSwitchNum);
                        }
                        if(buff->buff.isEmpty())
                        {
                            //结束发送
                            buff = (SendBuff_UDP *) m_WaitSendQueue.dequeue();
                            if(buff)
                            {
                                delete buff;
                                buff = NULL;
                            }
                            StopResendTimer();
                            StopCheckValidityTimer();
                            inSendWork = false;
                            slot_RestartSend();
                        }
                        else
                        {
                            if(FollowGroup.ucPackageSwitchNum<buff->totalPackIndex)
                            {
                                //按顺序继续发送后面的报文
                                SendSwitchFllowData(buff);
                                buff->usSendedNums = 1;
                                buff->lastSendedIndex = FollowGroup.ucPackageSwitchNum+1;
                            }
                            else
                            {
                                //检查还有那些报没有发送
                            }
                        }
                        StopResendTimer();
                        break;
                    }
                }
            }
            break;
        }//[~case:0x30]
    }
}

//发送不分组报文
qint64 RUDPSocket::SendNotSwitchData(bool isFile, const char *pData, int iSize, QHostAddress &host, quint16 port)
{
    GJB4908_UDP_NotSwitchGroup header;
    memset(&header,0,sizeof(GJB4908_UDP_NotSwitchGroup));

    header.transHeader.ucFlagAndType = 0x10;
    header.transHeader.usPackageLen = iSize + sizeof(GJB4908_UDP_NotSwitchGroup);
    header.transHeader.usPackageNo = QTime::currentTime().msecsTo(g_2000Time);
    QTime::currentTime().toString("hhmmsszzz").toUShort();
    header.transHeader.ucReserve = RAW;
    if(isFile)
    {
        header.messageArea.ucMessMarking = 0X10;
    }

    QByteArray SendData;
    SendData+=QByteArray((char*)&header,sizeof(GJB4908_UDP_NotSwitchGroup));
    SendData+=QByteArray(pData ,iSize);

    qint64 sendedSize = SendQtData(SendData.data(),SendData.size(),host,port);

    StartResendTimer();
    return sendedSize;
}

//发送分组报文-首报文
qint64 RUDPSocket::SendSwitchHeadData(bool isFile, const char *pData, int iSize, QHostAddress &host, quint16 port)
{
    GJB4908_UDP_HeadGroup header;
    memset(&header,0,sizeof(GJB4908_UDP_HeadGroup));

    header.transHeader.ucFlagAndType = 0x20;
    header.transHeader.usPackageLen = iSize + sizeof(GJB4908_UDP_HeadGroup);
    header.transHeader.usPackageNo = QTime::currentTime().toString("hhmmsszzz").toUShort();
    header.transHeader.ucReserve = RAW;
    header.ucPackageTotleNum = iSize/MAX_PACKAGE_SIZE +1;
    if(isFile)
    {
        header.messageArea.ucMessMarking = 0X10;
    }

    QByteArray headdata((char*)&header,sizeof(GJB4908_UDP_HeadGroup));
    QByteArray realdata(pData ,MAX_PACKAGE_SIZE);

    QByteArray SendData;
    SendData+=headdata;
    SendData+=realdata;

    qint64 sendedSize = SendQtData(SendData.data(),SendData.size(),host,port);
    StartResendTimer();
    return sendedSize;
}


//发送分组首报文-重载
qint64 RUDPSocket::SendSwitchHeadData(SendBuff_UDP *sendbuff)
{
    if(sendbuff==NULL)
    {
        return -1;
    }

    QByteArray waitSenddata(sendbuff->buff.value(1));

    GJB4908_UDP_HeadGroup header;
    memset(&header,0,sizeof(GJB4908_UDP_HeadGroup));

    header.transHeader.ucFlagAndType = 0x20;
    header.transHeader.usPackageLen = waitSenddata.size() + sizeof(GJB4908_UDP_HeadGroup);
    header.transHeader.usPackageNo = sendbuff->usPackageNo;
    header.transHeader.ucReserve = RAW;
    header.ucPackageTotleNum = sendbuff->totalPackIndex;
    if(sendbuff->isFile)
    {
        header.messageArea.ucMessMarking = 0X10;
    }

    QByteArray headdata((char*)&header,sizeof(GJB4908_UDP_HeadGroup));
    QByteArray SendData;
    SendData+=headdata;
    SendData+=waitSenddata;

    qint64 sendedSize = SendQtData(SendData.data(),SendData.size(),sendbuff->addr,sendbuff->port);
    StartResendTimer();
    return sendedSize;
}

//发送分组报文-后续报文
qint64 RUDPSocket::SendSwitchFllowData(bool isFile, unsigned short No, const char *pData, int iSize, int index,
                                       QHostAddress &host, quint16 port, int Type)
{
    GJB4908_UDP_FollowGroup header;
    memset(&header,0,sizeof(GJB4908_UDP_FollowGroup));

    header.transHeader.ucFlagAndType = 0x30;
    header.transHeader.usPackageLen = iSize + sizeof(GJB4908_UDP_FollowGroup);
    header.transHeader.usPackageNo = No;
    header.transHeader.ucReserve = RAW;
    header.ucPackageSwitchNum = index;

    QByteArray headdata((char*)&header,sizeof(GJB4908_UDP_FollowGroup));
    QByteArray realdata(pData ,iSize);

    QByteArray SendData;
    SendData+=headdata;
    SendData+=realdata;

    qint64 sendedSize = SendQtData(SendData.data(),SendData.size(),host,port);
    StartResendTimer();
    return sendedSize;
}

//发送分组报文-后续报文-重载
qint64 RUDPSocket::SendSwitchFllowData(SendBuff_UDP *sendbuff)
{
    if(sendbuff==NULL)
    {
        return -1;
    }

    QByteArray waitSenddata(sendbuff->buff.value(sendbuff->lastSendedIndex+1));
    GJB4908_UDP_FollowGroup header;
    memset(&header,0,sizeof(GJB4908_UDP_FollowGroup));

    header.transHeader.ucFlagAndType = 0x30;
    header.transHeader.usPackageLen = waitSenddata.size() + sizeof(GJB4908_UDP_FollowGroup);
    header.transHeader.usPackageNo = sendbuff->usPackageNo;
    header.transHeader.ucReserve = RAW;
    header.ucPackageSwitchNum = sendbuff->lastSendedIndex+1;

    QByteArray headdata((char*)&header,sizeof(GJB4908_UDP_FollowGroup));
    QByteArray SendData;
    SendData+=headdata;
    SendData+=waitSenddata;

    qint64 sendedSize = SendQtData(SendData.data(),SendData.size(),sendbuff->addr,sendbuff->port);

    StartResendTimer();
    return sendedSize;
}

//对发送的长报文进行分割
SendBuff_UDP *RUDPSocket::SplitePackageCount(const char *pData, int iSize)
{
    qint64 count = iSize/MAX_PACKAGE_SIZE + 1;
    SendBuff_UDP *buff = new SendBuff_UDP;
    buff->totalPackIndex = count;
    for(int i=1;i<=count;i++)
    {
        if(i==count)
        {
            buff->sendSize = iSize % MAX_PACKAGE_SIZE;
            char *tmp = new char[buff->sendSize];
            memcpy(tmp,pData+(i-1)*MAX_PACKAGE_SIZE,buff->sendSize);
            QByteArray data(tmp,buff->sendSize);
            buff->buff.insert(i,data);
        }
        else
        {
            buff->sendSize = MAX_PACKAGE_SIZE;
            char *tmp = new char[MAX_PACKAGE_SIZE];
            memcpy(tmp,pData+(i-1)*MAX_PACKAGE_SIZE,MAX_PACKAGE_SIZE);
            QByteArray data(tmp,MAX_PACKAGE_SIZE);
            buff->buff.insert(i,data);
        }
    }
    return buff;
}


//开始/重启重发计时器
void RUDPSocket::StartResendTimer()
{
    if(m_ResendTimer)
    {
        m_ResendTimer->start();
    }
}

//停止重发计时器
void RUDPSocket::StopResendTimer()
{
    inSendWork = false;
    if(m_ResendTimer && m_ResendTimer->isActive())
    {
        m_ResendTimer->stop();
    }
}

void RUDPSocket::StartCheckValidityTimer()
{
    if(m_CheckValidityTimer)
    {
        m_CheckValidityTimer->start();
    }
}

void RUDPSocket::StopCheckValidityTimer()
{
    if(m_CheckValidityTimer && m_CheckValidityTimer->isActive())
    {
        m_CheckValidityTimer->stop();
    }
}

//添加发送信息到队列
int RUDPSocket::AddSendBuffToQueue(SendBuff_UDP *sendbuff)
{
    m_WaitQueue.enqueue(sendbuff);
    return m_WaitQueue.size();
}

//从队列中取出发送信息
SendBuff_UDP *RUDPSocket::GetSendBuffFromQueue()
{
    SendBuff_UDP *buff = NULL;
    if(!m_WaitQueue.isEmpty())
    {
        buff = m_WaitQueue.dequeue();
    }
    return buff;
}

//重置发送状态，等待新的发送任务
int RUDPSocket::slot_RestartSend()
{
    int sendSize = -1;
    if(!inSendWork)
    {
        SendBuff_UDP * buff = GetSendBuffFromQueue();
        if(buff)
        {
            inSendWork = true;
            if(m_ResendTimer && m_ResendTimer->isActive())
            {
                m_ResendTimer->stop();
            }
            m_WaitSendQueue.clear();

            m_WaitSendQueue.enqueue(buff);
            sendSize = SendData(buff);
        }
    }
    return sendSize;
}

//对收到的不分组报文发送确认ACK信息
void RUDPSocket::AckNotSwitchGroup(GJB4908_UDP_NotSwitchGroup &header, QHostAddress &host, quint16 port)
{
    GJB4908_UDP_NotSwitchGroup ackdata;
    memcpy(&ackdata,&header,sizeof(GJB4908_UDP_NotSwitchGroup));
    ackdata.transHeader.ucReserve = ACK;
    int ret = this->SendQtData((char*)(&ackdata),sizeof(GJB4908_UDP_NotSwitchGroup),host,port);
}

//对收到的分组首报文发送确认ACK信息
void RUDPSocket::AckHeadGroup(GJB4908_UDP_HeadGroup &header, QHostAddress &host, quint16 port)
{
    GJB4908_UDP_HeadGroup ackdata;
    memcpy(&ackdata,&header,sizeof(GJB4908_UDP_HeadGroup));
    ackdata.transHeader.ucReserve = ACK;
    this->SendQtData((char*)(&ackdata),sizeof(GJB4908_UDP_HeadGroup),host,port);
}

//对收到的分组后续报文发送确认ACK信息
void RUDPSocket::AckFollowGroup(GJB4908_UDP_FollowGroup header, QHostAddress &host, quint16 port)
{
    GJB4908_UDP_FollowGroup ackdata;
    memcpy(&ackdata,&header,sizeof(GJB4908_UDP_FollowGroup));
    ackdata.transHeader.ucReserve = ACK;
    this->SendQtData((char*)(&ackdata),sizeof(GJB4908_UDP_FollowGroup),host,port);
}

//发送不分组报文-重载
qint64 RUDPSocket::SendNotSwitchData(SendBuff_UDP *sendbuff)
{
    if(sendbuff==NULL)
    {
        return -1;
    }
    //
    QByteArray waitSenddata(sendbuff->buff.value(1));

    GJB4908_UDP_NotSwitchGroup header;
    memset(&header,0,sizeof(GJB4908_UDP_NotSwitchGroup));

    header.transHeader.ucFlagAndType = 0x10;
    header.transHeader.usPackageLen = waitSenddata.size() + sizeof(GJB4908_UDP_NotSwitchGroup);
    header.transHeader.usPackageNo = sendbuff->usPackageNo;
    QTime::currentTime().toString("hhmmsszzz").toUShort();
    header.transHeader.ucReserve = RAW;
    if(sendbuff->isFile)
    {
        header.messageArea.ucMessMarking = 0X10;
    }

    QByteArray SendData;
    SendData+=QByteArray((char*)&header,sizeof(GJB4908_UDP_NotSwitchGroup));
    SendData+=waitSenddata;

    qint64 sendedSize = SendQtData(SendData.data(),SendData.size(),sendbuff->addr,sendbuff->port);

    StartResendTimer();

    return sendedSize;

}

//将完整的接收收据从队列中取出来
int RUDPSocket::FetckQueueData(int No)
{
    int returnSize;

    PacketBuff_UDP* Buff =  (PacketBuff_UDP*)m_RecvQueue.dequeue();
    if(Buff->isCompleted && Buff->packageNo == No)
    {
        m_LastRecvData = Buff->getFullData();
        returnSize = m_LastRecvData.size();

        if(m_lastRecvPacketBuff)
        {
            delete m_lastRecvPacketBuff;
            m_lastRecvPacketBuff = NULL;
            m_lastRecvPacketBuff = Buff;
        }
    }

    inSendWork = false;
    return returnSize;
}
