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

#include "network_global.h"
#include "rtask.h"

#include "rsocket.h"

namespace ClientNetwork{

class NETWORKSHARED_EXPORT MsgReceive : public RTask
{
    Q_OBJECT
public:
    explicit MsgReceive(QObject *parent = 0);
    ~MsgReceive();

    void setSock(RSocket * sock);

    void startMe();
    void stopMe();

signals:
    void socketError(int errorCode);

protected:
    void run();

private:
    QString errorString;

    RSocket * tcpSocket;
};

} //ClientNetwork

#endif // MSGRECEIVE_H
