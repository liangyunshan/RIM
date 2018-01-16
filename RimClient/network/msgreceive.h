/*!
 *  @brief     文本信息接收
 *  @details   接收文本信息，接收后直接保存由其它程序处理
 *  @file      msgreceive.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGRECEIVE_H
#define MSGRECEIVE_H

#include <QThread>
#include "network_global.h"

#include "rsocket.h"

namespace ClientNetwork{

class NETWORKSHARED_EXPORT MsgReceive : public QThread
{
    Q_OBJECT
public:
    explicit MsgReceive(RSocket tcpSocket,QObject *parent = 0);
    ~MsgReceive();

    void startMe();
    void setRunning(bool flag);

    void stop();

protected:
    void run();

private:
    QString errorString;

    bool runningFlag;

    RSocket tcpSocket;
};

} //ClientNetwork

#endif // MSGRECEIVE_H
