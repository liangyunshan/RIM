/*!
 *  @brief     rudpsocket.h
 *  @details   封装UDP可靠传输
 *  @author    尚超
 *  @version   1.0
 *  @date      2018.06.06
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef RUDPSOCKET_H
#define RUDPSOCKET_H

#include <QObject>
#include <QHostAddress>
#include <QLinkedList>
#include <QQueue>
#include <QByteArray>

#include "../Core/localprotocoldata.h"
#include "../network_global.h"
using namespace GJB4908;

class QUdpSocket;
class QTimer;

//信息发送类型
enum AckType{
    RAW=0,      //最原始的用户数据
    ACK=1,      //接收方确定收到信息
};

//UDP数据包接收缓冲
struct PacketBuff_UDP
{
    PacketBuff_UDP()
    {
        totalPackIndex = 0;
        recvSize = 0;
        packageNo = 0;
        isCompleted = false;
        isSorted= false;
    }

    QByteArray getFullData()
    {
        if(!isSorted)
        {
            Sort();
        }
        return data;
    }

    QByteArray Sort()
    {
        data.clear();
        if(isCompleted && recvSize > 0 && buff.size()>0)
        {
            int i = 1;
            while(!buff.isEmpty())
            {
                for(int index = 0 ;index<buff.size();index++)
                {
                    if(buff.contains(i))
                    {
                        data.append(buff.value(i));
                        buff.remove(i);
                        ++i;
                        break;
                    }
                }
            }
        }
        buff.clear();
        isSorted = true;
        return data;
    }
    int getPackageSizeof()
    {
        int size = 0;
        size += sizeof(bool);
        size += sizeof(unsigned int);
        size += sizeof(unsigned short);
        size += sizeof(unsigned short);
        size += sizeof(QHostAddress);
        size += sizeof(quint16);
    }

    bool isCompleted;                           //该缓冲数据是否完整
    unsigned int recvSize;                      //接收数据的长度
    unsigned short totalPackIndex;              //总数据包分包大小
    unsigned short packageNo;
    bool isSorted;
    QHostAddress addr;
    quint16 port;
    QHash<unsigned short,QByteArray> buff;
    QByteArray data;
};

//UDP发送包缓冲
struct SendBuff_UDP
{
    SendBuff_UDP()
    {
        totalPackIndex = 0;
        sendSize = 0;
        sendedPackNums = 0;
        lastSendedIndex = 0;
        usPackageNo = 0;
        usSendedNums =0;
        isFile = false;
    }

    unsigned int sendSize;                      //数据总的长度
    unsigned short totalPackIndex;              //分包数量
    unsigned short sendedPackNums;              //已发送包数量
    unsigned short lastSendedIndex ;            //最新发送的包序号
    unsigned short usPackageNo;                 //包流水号
    unsigned short usSendedNums;                //基于这一包已经发送了多少次，每发送一次新报，次数设置为1，重发一次次数加1
    bool isFile;                                //文件传输标识 false 0：非文件传输   true 1：文件传输
    QHostAddress addr;
    quint16 port;
    QHash<unsigned short,QByteArray> buff;
};

class NETWORKSHARED_EXPORT RUDPSocket : public QObject
{
    Q_OBJECT
public:
    explicit RUDPSocket(QHostAddress host, int localRecvPort, QObject *parent = nullptr);
    ~RUDPSocket();
    qint64 SendData(bool isFile, const char *pData, int iSize, QHostAddress &host , quint16 port);
    qint64 SendData(SendBuff_UDP* buff);
    qint64 SendFile(QString filename, QHostAddress &host , quint16 port);
    qint64 SendQtData(const char *pData, int iSize, QHostAddress &host , quint16 port);
    qint64 readDatagram(char *data, qint64 *maxlen, QHostAddress *host = Q_NULLPTR, quint16 *port = Q_NULLPTR);
    QByteArray readLastData();
    bool isInSendWork()
    {
        return inSendWork;
    }

signals:
    void readyRead();
    void readyRead(QByteArray);
    void readyReadFile(QString filepath);
    void readyToSendBuff();

private slots:
    void slot_ResendTimerOut();
    void slot_CheckValidityTimerOut();
    void readPendingDatagrams();
    int  slot_RestartSend();

private:
    QUdpSocket *m_udpRecvScocket;
    quint16 m_RecvPort;
    QUdpSocket *m_udpSendScocket;
    QQueue<SendBuff_UDP*> m_WaitSendQueue;
    QQueue<PacketBuff_UDP*> m_RecvQueue;
    QTimer *m_ResendTimer;  //发送时的报文重发定时器
    QTimer *m_CheckValidityTimer;//对接收数据有效性判断的定时器
    QByteArray m_LastRecvData;
    PacketBuff_UDP* m_lastRecvPacketBuff;

private:
    void AckNotSwitchGroup(GJB4908_UDP_NotSwitchGroup &header, QHostAddress &host , quint16 port);
    void AckHeadGroup(GJB4908_UDP_HeadGroup &header, QHostAddress &host , quint16 port);
    void AckFollowGroup(GJB4908_UDP_FollowGroup  header, QHostAddress &host , quint16 port);

    qint64 SendNotSwitchData(bool isFile,const char *pData, int iSize, QHostAddress &host, quint16 port);
    qint64 SendNotSwitchData(SendBuff_UDP *sendbuff);
    qint64 SendSwitchHeadData(bool isFile, const char *pData, int iSize, QHostAddress &host , quint16 port);
    qint64 SendSwitchHeadData(SendBuff_UDP *sendbuff);
    qint64 SendSwitchFllowData(bool isFile, unsigned short No, const char *pData, int iSize, int index, QHostAddress &host , quint16 port, int Type=ACK);
    qint64 SendSwitchFllowData(SendBuff_UDP* sendbuff);
    SendBuff_UDP* SplitePackageCount(const char *pData, int iSize);
    int FetckQueueData(int No);

    void StartResendTimer();
    void StopResendTimer();

    void StartCheckValidityTimer();
    void StopCheckValidityTimer();
    int AddSendBuffToQueue(SendBuff_UDP *sendbuff);
    SendBuff_UDP*  GetSendBuffFromQueue();
    QQueue<SendBuff_UDP*> m_WaitQueue;


    bool inSendWork;

};

#endif // RUDPSOCKET_H
