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

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "rsocket.h"

namespace ClientNetwork{

class NETWORKSHARED_EXPORT MsgSender : public QThread
{
    Q_OBJECT
public:
    explicit MsgSender(RSocket socket,QThread * parent = 0);

    void startMe();
    void setRunning(bool flag);

protected:
    void run();

private:
    void handleDataSend(QByteArray & data);

private:
    RSocket socket;

    bool runningFlag;
};

}

#endif // MSGSENDER_H
