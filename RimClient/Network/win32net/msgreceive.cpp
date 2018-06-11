#include "msgreceive.h"

#include <QUdpSocket>
#include <QDebug>

#ifdef Q_OS_WIN
#include <winsock2.h>
#endif

#include "Util/rlog.h"
#include "Util/rutil.h"
#include "netglobal.h"
#include "../multitransmits/basetransmit.h"

#define MSG_RECV_BUFF 1024

namespace ClientNetwork{

RecveiveTask::RecveiveTask(QObject *parent) :
    transmit(NULL),RTask(parent)
{

}

void RecveiveTask::bindTransmit(BaseTransmit *trans)
{
    transmit = trans;
}

void RecveiveTask::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
}

void RecveiveTask::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
    }
}

void RecveiveTask::run()
{
    runningFlag = true;

    char recvBuff[MSG_RECV_BUFF] = {0};
    std::function<void(QByteArray&)> func = std::bind(&ClientNetwork::RecveiveTask::processData,this,std::placeholders::_1);
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

RecveiveTask::~RecveiveTask()
{

}

TextReceive::TextReceive(QObject *parent):
    RecveiveTask(parent)
{

}

TextReceive::~TextReceive()
{
    stopMe();
    wait();
}

/*!
 * @brief 文本接收线程处理
 * @warning 接收由网络层拼接后的完整数据包，用户可在此处进行数据包处理，建议不要做复杂的逻辑运算，否则会影响数据接收。
 * @param[in] data 待处理的数据
 */
void TextReceive::processData(QByteArray &data)
{
    G_TextRecvMutex.lock();
    G_TextRecvBuffs.enqueue(data);
    G_TextRecvMutex.unlock();

    G_TextRecvCondition.wakeOne();
}

FileReceive::FileReceive(QObject *parent):
    RecveiveTask(parent)
{

}

FileReceive::~FileReceive()
{
    stopMe();
    wait();
}

void FileReceive::processData(QByteArray &data)
{
    G_FileRecvMutex.lock();
    G_FileRecvBuffs.enqueue(data);
    G_FileRecvMutex.unlock();

    G_FileRecvCondition.wakeOne();
}

} //namespace ClientNetwork;
