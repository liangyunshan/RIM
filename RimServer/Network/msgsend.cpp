#include "msgsend.h"

#include <QUdpSocket>

#include "Util/rlog.h"

MsgSend::MsgSend(QObject *parent) : QObject(parent)
{

}

MsgSend::~MsgSend()
{

}


void MsgSend::setPort(quint16 port)
{
    this->port = port;
}

bool MsgSend::init()
{

    return true;
}
