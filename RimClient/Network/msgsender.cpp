#include "msgsender.h"

#include <QDebug>
#include <QDateTime>

#include "netglobal.h"
#include "Util/rlog.h"

namespace ClientNetwork{

MsgSender::MsgSender(RSocket socket, QThread *parent):
    socket(socket),QThread(parent)
{
    runningFlag = false;
}

void MsgSender::startMe()
{
    if(!isRunning())
    {
        start();
    }
    else
    {
        G_SendWaitCondition.wakeOne();
    }
}

void MsgSender::setRunning(bool flag)
{
    runningFlag = flag;
}

void MsgSender::run()
{
    runningFlag = true;

//    char * test = new char[200];

//    for(int i = 0; i<1000;i++)
//    {
//        for(int i=0;i<200;i++)
//        {
//            test[i] = 'a'+qrand()%10;
//        }
//        QByteArray array;
//        array.setRawData((char *)test,200);
//        G_SendBuff.enqueue(array);
//    }

    RLOG_INFO("Start send++++++++++++++++++++++");
    while(runningFlag)
    {
        if(G_SendBuff.size() <= 0)
        {
            G_SendMutex.lock();
            G_SendWaitCondition.wait(&G_SendMutex);
            G_SendMutex.unlock();
        }

        G_SendMutex.lock();
        QByteArray data =  G_SendBuff.dequeue();
        G_SendMutex.unlock();

        handleDataSend(data);
    }
    RLOG_INFO("Stop send++++++++++++++++++++++");
}

void MsgSender::handleDataSend(QByteArray &data)
{
    if(socket.isValid())
    {
       int len = socket.send(data.data(),data.length());
       qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"**"<<data.data()<<"**";
       qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"=============="<<len;
       if(len <= 0)
       {
            RLOG_ERROR("Send a data error!");
            socket.closeSocket();
       }
    }
}

} //ClientNetwork
