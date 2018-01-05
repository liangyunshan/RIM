/*!
 *  @brief     文本信息发送
 *  @details   用于发送文本信息
 *  @file      socket.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.05
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>

class QUdpSocket;

class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(QObject * parent = 0);
    ~Socket();

    void setPort(quint16 port);

    bool init();

private:
    quint16 port;

    QUdpSocket * udpSocket;

};

#endif // SOCKET_H
