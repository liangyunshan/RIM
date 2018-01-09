/*!
 *  @brief     服务器端
 *  @details   用于接受客户端的请求，并未每个客户端分配单独的处理
 *  @file      tcpserver.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.09
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);


};

#endif // TCPSERVER_H
