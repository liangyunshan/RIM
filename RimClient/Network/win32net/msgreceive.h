/*!
 *  @brief     文本信息接收
 *  @details   接收文本信息，接收后直接保存由其它程序处理
 *  @file      msgreceive.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.06
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note   20180123:wey:调整客户端支持分包接收、组包
 *          20180305:wey:调整接收结构，RecveiveTask完全封装数据组包接收，子类负责对数据进行处理
 */
#ifndef MSGRECEIVE_H
#define MSGRECEIVE_H

#include <QHash>

#include "../network_global.h"
#include "../rtask.h"
#include "../rsocket.h"

namespace ClientNetwork{

class NETWORKSHARED_EXPORT RecveiveTask : public RTask
{
    Q_OBJECT
public:
    explicit RecveiveTask(QObject *parent = 0);
    virtual ~RecveiveTask();

    void setSock(RSocket * sock);

    void startMe();
    void stopMe();

signals:
    void socketError(int errorCode);

protected:
    void run();
    void recvData(char * recvData,int recvLen);
    virtual void processData(QByteArray & data)=0;

protected:
    QString errorString;

    RSocket * tcpSocket;
    QByteArray lastRecvBuff;                        //断包接收缓冲区
    QHash<int,PacketBuff*> packetBuffs;             //多包缓冲区
};

class NETWORKSHARED_EXPORT TextReceive : public RecveiveTask
{
    Q_OBJECT
public:
    explicit TextReceive(QObject *parent = 0);
    ~TextReceive();

private:
    void processData(QByteArray & data);
};

class NETWORKSHARED_EXPORT FileReceive : public RecveiveTask
{
    Q_OBJECT
public:
    explicit FileReceive(QObject * parent = 0);
    ~FileReceive();

private:
    void processData(QByteArray & data);
};

} //ClientNetwork

#endif // MSGRECEIVE_H
