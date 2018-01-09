/*!
 *  @brief     信息发送
 *  @details   向各个客户端发送信息
 *  @file      msgsend.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.08
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGSEND_H
#define MSGSEND_H

#include <QObject>

#include "network_global.h"

class NETWORKSHARED_EXPORT MsgSend : public QObject
{
    Q_OBJECT
public:
    explicit MsgSend(QObject *parent = 0);
    ~MsgSend();

    void setPort(quint16 port);

    bool init();

private:
    quint16 port;

};

#endif // MSGSEND_H
