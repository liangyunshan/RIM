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

#include "rtask.h"
#include "rsocket.h"

namespace ClientNetwork{

class NETWORKSHARED_EXPORT MsgSender : public RTask
{
    Q_OBJECT
public:
    explicit MsgSender(QThread * parent = 0);

    void setSock(RSocket *sock);

    void startMe();
    void stopMe();

signals:
    void socketError(int errorCode);

protected:
    void run();

private:
    bool handleDataSend(QByteArray & data);

private:
    RSocket* tcpSocket;
};

}

#endif // MSGSENDER_H
