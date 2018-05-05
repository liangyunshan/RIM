/*!
 *  @brief     信息发送
 *  @details   负责信息的发送，从信息队列中取出信息逐条发送。
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.XX
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGSENDER_H
#define MSGSENDER_H

#include <QMutex>
#include <QWaitCondition>

#include "../rtask.h"
#include "../rsocket.h"

#define MAX_PACKET 1024                  //发送数据时一次最大数据长度(不包括前后的控制信息)
#define MAX_SEND_BUFF (MAX_PACKET + 24)

namespace ClientNetwork{

class NETWORKSHARED_EXPORT SendTask : public RTask
{
    Q_OBJECT
public:
    explicit SendTask(QThread * parent = 0);

    void setSock(RSocket *sock);

signals:
    void socketError(int errorCode);

protected:
    void run();

    virtual void processData()=0;
    virtual bool handleDataSend(QByteArray & data)=0;

protected:
    RSocket* tcpSocket;
    char sendBuff[MAX_SEND_BUFF];
    int SendPackId;
    QMutex SendPackMutex;
};

class NETWORKSHARED_EXPORT TextSender : public SendTask
{
    Q_OBJECT
public:
    explicit TextSender(QThread * parent = 0);
    ~TextSender();

    void startMe();
    void stopMe();

protected:
    void processData();
    bool handleDataSend(QByteArray & data);
};

class NETWORKSHARED_EXPORT FileSender : public SendTask
{
    Q_OBJECT
public:
    explicit FileSender(QThread * parent = 0);
    ~FileSender();

    void startMe();
    void stopMe();

protected:
    void processData();
    bool handleDataSend(QByteArray & data);
};

}

#endif // MSGSENDER_H
