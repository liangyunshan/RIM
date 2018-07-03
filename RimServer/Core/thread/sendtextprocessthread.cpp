#include "sendtextprocessthread.h"

#include <QDebug>
#include <qmath.h>

#include "Network/netglobal.h"
#include "Network/win32net/netutils.h"
#include "Network/tcpclient.h"
#include "Network/win32net/iocpcontext.h"
#include "Network/win32net/tcpserver.h"
#include "Util/rlog.h"

#include "Network/multitransmits/tcptransmit.h"

using namespace ServerNetwork;

SendTextProcessThread::SendTextProcessThread()
{
    runningFlag = false;

    initTransmits();
}

SendTextProcessThread::~SendTextProcessThread()
{
    wait();
}

/*!
 * @brief 初始化数据传输链路
 */
void SendTextProcessThread::initTransmits()
{
    std::shared_ptr<TcpTransmit> tcpTrans = std::make_shared<TcpTransmit>();
    transmits.insert(std::make_pair<CommMethod,BaseTransmit_Ptr>(tcpTrans->type(),tcpTrans));
}

void SendTextProcessThread::run()
{
    runningFlag = true;

    while(runningFlag)
    {
        while(G_SendButts.size() == 0)
        {
            G_SendCondition.wait(std::unique_lock<std::mutex>(G_SendMutex));
        }

        if(runningFlag)
        {
            SendUnit sockData;

            G_SendMutex.lock();
            if(G_SendButts.size() > 0)
            {
               sockData =  G_SendButts.front();
               G_SendButts.pop();
            }
            G_SendMutex.unlock();

            if(!handleDataSend(sockData)){
                //TODO 对错误处理
            }
        }
    }
}

bool SendTextProcessThread::handleDataSend(SendUnit &unit)
{
    auto selectedTrans = transmits.find(unit.method);
    if( selectedTrans == transmits.end())
        return false;

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
           <<"unit.dataUnit.data:"<<unit.dataUnit.data
          <<"\n";
    return (*selectedTrans).second->startTransmit(unit);
}
