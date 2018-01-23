#include "msgreceive.h"

#include <QUdpSocket>
#include <QDebug>

#include "Util/rlog.h"
#include "Util/rutil.h"
#include "netglobal.h"

#define MSG_RECV_BUFF 1024

namespace ClientNetwork{

MsgReceive::MsgReceive(QObject *parent) :
    tcpSocket(NULL),RTask(parent)
{

}

MsgReceive::~MsgReceive()
{

}

void MsgReceive::setSock(RSocket *sock)
{
    tcpSocket = sock;
}

void MsgReceive::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
}

void MsgReceive::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
    }
}

void MsgReceive::run()
{
    runningFlag = true;
    RLOG_INFO("Start Receive++++++++++++++++++++++");
    while(runningFlag)
    {
        char buff[MSG_RECV_BUFF] = {0};
        int ret = tcpSocket->recv(buff,MSG_RECV_BUFF);
        if(ret > 0)
        {
            QByteArray data(buff,ret);

            G_RecvMutex.lock();
            G_RecvButts.enqueue(data);
            G_RecvMutex.unlock();

            G_RecvCondition.wakeOne();
        }
        else
        {
            RLOG_ERROR("receive a pack not completely %d",tcpSocket->getLastError());
            tcpSocket->closeSocket();
            emit socketError(tcpSocket->getLastError());
            break;
        }
    }

    runningFlag = false;
    RLOG_INFO("Stop Receive++++++++++++++++++++++");
}

} //namespace ClientNetwork;
