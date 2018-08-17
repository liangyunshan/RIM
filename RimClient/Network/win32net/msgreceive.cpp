#include "msgreceive.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <winsock2.h>
#endif

#include <assert.h>

#include "Util/rlog.h"
#include "Util/rutil.h"
#include "netglobal.h"
#include "../multitransmits/basetransmit.h"

#define MSG_RECV_BUFF 1024

namespace ClientNetwork{

DataRecveiveTask::DataRecveiveTask(QObject *parent) :
    transmit(NULL),RTask(parent)
{
}

void DataRecveiveTask::bindTransmit(std::shared_ptr<BaseTransmit> trans)
{
    transmit = trans;
}

void DataRecveiveTask::registDataHandle(DataHandler handler)
{
    dataHandler = handler;
}

void DataRecveiveTask::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
}

void DataRecveiveTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
}

void DataRecveiveTask::run()
{
    assert(transmit != NULL);

    runningFlag = true;

    char recvBuff[MSG_RECV_BUFF] = {0};
    std::function<void(RecvUnit&)> func = std::bind(&ClientNetwork::DataRecveiveTask::processData,this,std::placeholders::_1);
    while(runningFlag)
    {
        memset(recvBuff,0,MSG_RECV_BUFF);
        bool success = transmit->startRecv(recvBuff,MSG_RECV_BUFF,func);
        if(!success){
            emit socketError(transmit->type());
            break;
        }
    }

    runningFlag = false;
}

void DataRecveiveTask::processData(RecvUnit &data)
{
    if(dataHandler)
        dataHandler(data);
}

DataRecveiveTask::~DataRecveiveTask()
{

}

} //namespace ClientNetwork;
