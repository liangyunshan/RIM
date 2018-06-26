/*!
 *  @brief     信息发送
 *  @details   负责信息的发送，从信息队列中取出信息逐条发送。 \n
 *             对于发送线程，其可以【添加】多种传输方式，应用层将待发送的数据以及发送方式组合后，压入发送队列。\n
 *             发送线程从队列中依次取出数据，根据数据的发送方式，选择对应Transmit方式，由其内部自主发送，只需反馈是否正确传输 \n
 *             若传输错误，向应用层发送对应方式的错误信号。
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.10
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGSENDER_H
#define MSGSENDER_H

#include "../rtask.h"
#include "../rsocket.h"

#include <memory>
#include <mutex>
#include <map>

namespace ClientNetwork{

class BaseTransmit;

class NETWORKSHARED_EXPORT SendTask : public RTask
{
    Q_OBJECT
public:
    explicit SendTask(QThread * parent = 0);

    bool addTransmit(std::shared_ptr<BaseTransmit> trans);
    bool removaAllTransmit();

signals:
    void socketError(CommMethod method);

protected:
    void run();

    virtual void processData()=0;
    virtual bool handleDataSend(SendUnit &unit)=0;

protected:
    char sendBuff[MAX_SEND_BUFF];
    int SendPackId;
    QMutex SendPackMutex;

    std::mutex tranMutex;
    std::map<CommMethod,std::shared_ptr<BaseTransmit> > transmits;
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

};

}

#endif // MSGSENDER_H
