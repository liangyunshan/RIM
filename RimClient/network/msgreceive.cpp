#include "msgreceive.h"

#include <QUdpSocket>
#include <QDebug>

#include <WinSock2.h>

#include "Util/rlog.h"
#include "Util/rutil.h"
#include "netglobal.h"

#define MSG_RECV_BUFF 1024

namespace ClientNetwork{

MsgReceive::MsgReceive(RSocket tcpSocket, QObject *parent) :
    tcpSocket(tcpSocket),QThread(parent)
{
    runningFlag = false;
}

MsgReceive::~MsgReceive()
{
    stop();
}

void MsgReceive::startMe()
{
    if(!isRunning())
    {
        start();
    }
}

void MsgReceive::stop()
{
    runningFlag = false;
}

void MsgReceive::run()
{
    runningFlag = true;
    RLOG_INFO("Start Receive++++++++++++++++++++++");
    while(runningFlag)
    {
        char buff[MSG_RECV_BUFF] = {0};
        int ret = tcpSocket.recv(buff,MSG_RECV_BUFF);
        if(ret > 0)
        {
            QByteArray data;
            data.setRawData(buff,ret);

            G_RecvMutex.lock();
            G_RecvButts.enqueue(data);
            G_RecvMutex.unlock();

            static int index = 0;

            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<buff<<ret<<index++;

            G_RecvCondition.wakeOne();
        }
        else
        {
            RLOG_ERROR("receive a pack not completely %d",GetLastError());
            tcpSocket.closeSocket();
            break;
        }
    }

    RLOG_INFO("Stop Receive++++++++++++++++++++++");
}

} //namespace ClientNetwork;
