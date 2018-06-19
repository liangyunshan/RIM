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

#include "../rtask.h"
#include "../rsocket.h"

#include <memory>
#include <mutex>
#include <map>
#include "../wraprule/datapacketrule.h"

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
