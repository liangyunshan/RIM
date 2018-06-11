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

#include <memory>
#include "../wraprule/datapacketrule.h"

class RAES;
class RUDPSocket;

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
    virtual bool handleDataSend(SendUnit &unit)=0;

protected:
    RSocket* tcpSocket;
    char sendBuff[MAX_SEND_BUFF];
    int SendPackId;
    QMutex SendPackMutex;
    RAES *m_RAES;
    RUDPSocket *m_pRUDPSendSocket;
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
    bool handleDataSend(SendUnit &unit);

private:
    std::shared_ptr<DataPacketRule> dataPacketRule;
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
    bool handleDataSend(SendUnit &unit);

private:
    std::shared_ptr<DataPacketRule> dataPacketRule;
};

}

#endif // MSGSENDER_H
