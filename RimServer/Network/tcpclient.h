/*!
 *  @brief     客户端
 *  @details   封装了Tcp的用户请求信息
 *  @file      tcpclient.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.09
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = 0);

};

#endif // TCPCLIENT_H
