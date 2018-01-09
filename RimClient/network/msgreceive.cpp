#include "msgreceive.h"

#include <QUdpSocket>

#include "Util/rlog.h"
#include "Util/rutil.h"

#include "netglobal.h"

MsgReceive::MsgReceive(QObject *parent) : QObject(parent),
    socket(NULL)
{

}

MsgReceive::~MsgReceive()
{
    stop();
}

bool MsgReceive::init(QString address, quint16 port)
{
    if(!socket)
    {
        socket = new QUdpSocket();
        connect(socket,SIGNAL(readyRead()),this,SLOT(processData()));
    }

    if(!socket->bind(QHostAddress(address),port))
    {
        RLOG_ERROR("receive socket bind error, address %s,port %d",address.toLocal8Bit().data(),port);
        return false;
    }
    RLOG_INFO("receive bind success");

    return true;
}

void MsgReceive::stop()
{
    if(socket)
    {
        socket->close();
    }
}

void MsgReceive::processData()
{
    while(socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        qint64 recvlen = socket->readDatagram(datagram.data(),datagram.size(),&sender,&senderPort);
        if(recvlen == datagram.size())
        {
            G_RecvMutex.lock();
            G_RecvButts.enqueue(datagram);
            G_RecvMutex.unlock();

            G_RecvCondition.wakeOne();
        }
        else
        {
            RLOG_ERROR("receive a pack not completely %d",RUtil::currentMSecsSinceEpoch());
        }
    }
}
