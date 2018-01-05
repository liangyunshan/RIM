#include "socket.h"

#include <QUdpSocket>

#include "Util/rlog.h"

Socket::Socket(QObject *parent):
    udpSocket(NULL),
    QObject(parent)
{

}

Socket::~Socket()
{

}

void Socket::setPort(quint16 port)
{
    this->port = port;
}

bool Socket::init()
{
    if(!udpSocket)
    {
        udpSocket = new QUdpSocket();
    }
    return true;
}
