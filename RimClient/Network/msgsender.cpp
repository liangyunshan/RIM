#include "msgsender.h"

#include <QDebug>
#include <QDateTime>

#include "netglobal.h"
#include "Util/rlog.h"

namespace ClientNetwork{

MsgSender::MsgSender(QThread *parent):tcpSocket(NULL),
   RTask(parent)
{

}

void MsgSender::setSock(RSocket * sock)
{
    tcpSocket = sock;
}

void MsgSender::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
    else
    {
        G_SendWaitCondition.wakeOne();
    }
}

void MsgSender::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
        G_SendWaitCondition.wakeOne();
    }
}

void MsgSender::run()
{
    runningFlag = true;

    RLOG_INFO("Start Send++++++++++++++++++++++");
    while(runningFlag)
    {
        if(G_SendBuff.size() <= 0)
        {
            G_SendMutex.lock();
            G_SendWaitCondition.wait(&G_SendMutex);
            G_SendMutex.unlock();
        }
        if(runningFlag)
        {
            G_SendMutex.lock();
            QByteArray data =  G_SendBuff.dequeue();
            G_SendMutex.unlock();

            if(!handleDataSend(data))
            {
                emit socketError(tcpSocket->getLastError());
                break;
            }
        }
    }
    runningFlag = false;

    RLOG_INFO("Stop Send++++++++++++++++++++++");
}

bool MsgSender::handleDataSend(QByteArray &data)
{
    if(tcpSocket->isValid() && tcpSocket->send(data.data(),data.length()) > 0)
    {
       return true;
    }

    RLOG_ERROR("Send a data error!");
    tcpSocket->closeSocket();

    return false;
}

} //ClientNetwork
