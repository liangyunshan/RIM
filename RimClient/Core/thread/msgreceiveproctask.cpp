#include "msgreceiveproctask.h"

#include "network/netglobal.h"
#include "rsingleton.h"
#include "head.h"
#include "jsonkey.h"
#include "messdiapatch.h"
#include "Network/msgparse/msgparsefactory.h"

#include "../Network/wraprule/tcp_wraprule.h"

MsgReceiveProcTask::MsgReceiveProcTask(QObject *parent):
    ClientNetwork::RTask(parent)
{

}

MsgReceiveProcTask::~MsgReceiveProcTask()
{
    stopMe();
    wait();
}

void MsgReceiveProcTask::startMe()
{
    RTask::startMe();

    runningFlag = true;

    if(!isRunning())
    {
        start();
    }
    else
    {
        G_TextRecvCondition.notify_one();
    }
}

void MsgReceiveProcTask::run()
{
    while(runningFlag)
    {
        while(runningFlag && G_TextRecvBuffs.empty())
        {
            std::unique_lock<std::mutex> ul(G_TextRecvMutex);
            G_TextRecvCondition.wait(ul);
        }

        if(runningFlag && G_TextRecvBuffs.size() > 0)
        {
            G_TextRecvMutex.lock();
            RecvUnit array = G_TextRecvBuffs.front();
            G_TextRecvBuffs.pop();
            G_TextRecvMutex.unlock();

            if(array.data.size() > 0)
            {
                validateRecvData(array);
            }
        }
    }
}

/*!
 * @brief 向窗口的工具栏中插入工具按钮，默认是自左向右排列
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void MsgReceiveProcTask::validateRecvData(const RecvUnit &data)
{
    ProtocolPackage packData;
#ifdef __LOCAL_CONTACT__
    bool result = false;
    switch(data.extendData.method){
        case C_TCP:
            result = RSingleton<TCP_WrapRule>::instance()->unwrap(data.data,packData);
        break;

        default:
            break;
    }
    if(result)
        RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(packData);
#else
    packData.data = data.data;
    RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(packData);
#endif

}

void MsgReceiveProcTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    G_TextRecvCondition.notify_one();
}
