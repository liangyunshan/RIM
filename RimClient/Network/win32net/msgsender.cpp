#include "msgsender.h"

#include "netglobal.h"
#include "Util/rlog.h"
#include "../multitransmits/basetransmit.h"

namespace ClientNetwork{

SendTask::SendTask(QThread *parent):
    RTask(parent)
{
}

/*!
 * @brief 添加新的传输方式
 * @param[in] BaseTransmit * 待添加传输方式对象
 * @return 是否添加成功
 * @attention 若容器中已经存在此种传输方式，默认不替换
 */
bool SendTask::addTransmit(std::shared_ptr<BaseTransmit> trans)
{
    std::lock_guard<std::mutex> lg(tranMutex);
    if(transmits.find(trans->type()) != transmits.end())
        return true;

    transmits.insert(std::pair<CommMethod,std::shared_ptr<BaseTransmit>>(trans->type(),trans));
    return true;
}

/*!
 * @brief 清空所有的传输方式
 * @return 是否全部清空
 */
bool SendTask::removaAllTransmit()
{
    std::lock_guard<std::mutex> lg(tranMutex);

    auto tbegin = transmits.begin();
    while(tbegin != transmits.end()){
        (*tbegin).second->close();
        (*tbegin).second.reset();
        tbegin = transmits.erase(tbegin);
    }

    return true;
}

void SendTask::run()
{
    runningFlag = true;

    RLOG_INFO("Start Send++++++++++++++++++++++");
    processData();
    runningFlag = false;

    RLOG_INFO("Stop Send++++++++++++++++++++++");
}

TextSender::TextSender(QThread *parent):
   SendTask(parent)
{

}

TextSender::~TextSender()
{
    stopMe();
    wait();
//    removaAllTransmit();
}

void TextSender::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
    else
    {
        G_TextSendWaitCondition.notify_one();
    }
}

void TextSender::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
        G_TextSendWaitCondition.notify_one();
    }
}

void TextSender::processData()
{
    while(runningFlag)
    {
        while(runningFlag && G_TextSendBuffs.size() <= 0)
        {
            std::unique_lock<std::mutex> ul(G_TextSendMutex);
            G_TextSendWaitCondition.wait(ul);
        }
        if(runningFlag)
        {
            G_TextSendMutex.lock();
            SendUnit unit =  G_TextSendBuffs.front();
            G_TextSendBuffs.pop();
            G_TextSendMutex.unlock();

            if(!handleDataSend(unit))
            {
                emit socketError(unit.method);
                break;
            }
        }
    }
}

/*!
 * @brief 向服务器发送指定数据
 * @details 此时需要在数据内容前加入数据头信息，用于控制分包发送时的序列
 * @param[in] data 待发送数据内容
 * @return 是否完全发送成功
 */
bool TextSender::handleDataSend(SendUnit &unit)
{
    std::lock_guard<std::mutex> lg(tranMutex);

    auto iter = transmits.find(unit.method);
    if(iter != transmits.end()){
        if((*iter).second.get() != nullptr && (*iter).second->connected())
            return (*iter).second->startTransmit(unit);
    }

    return false;
}

FileSender::FileSender(QThread *parent):
    SendTask(parent)
{

}

FileSender::~FileSender()
{
//    removaAllTransmit();
    stopMe();
    wait();
}

void FileSender::startMe()
{
    RTask::startMe();
    if(!isRunning())
    {
        start();
    }
    else
    {
        G_FileSendWaitCondition.notify_one();
    }
}

void FileSender::stopMe()
{
    if(runningFlag)
    {
        RTask::stopMe();
        runningFlag = false;
        G_FileSendWaitCondition.notify_one();
    }
}

void FileSender::processData()
{
    while(runningFlag)
    {
        while(G_FileSendBuffs.size() <= 0)
        {
            if(!runningFlag)
                break;

            std::unique_lock<std::mutex> ul(G_FileSendMutex);
            G_FileSendWaitCondition.wait(ul);
        }

        if(runningFlag)
        {
            G_FileSendMutex.lock();
            SendUnit sendUnit =  G_FileSendBuffs.front();
            G_FileSendBuffs.pop();
            G_FileSendMutex.unlock();

            if(!handleDataSend(sendUnit))
            {
                emit socketError(sendUnit.method);
                break;
            }
        }
    }
}

/*!
 * @brief 处理数据发送
 * @details 网络层接收应用层传输数据后，在数据的前增加了网络层的标识，负责网络层数据的可靠投递; @n
 *          每次发送时会将应用层数据分配一个自增的ID(SendPackId)，用于表示应用层数据的身份信息(不管当前数据包长度的大小); @n
 *          若应用层数据包，超过了网络层一次传输的阈值，则对该应用层数据进行分子包传输，每个子包按照从0开始自增索引，用于控制传输的序列; @n
 * @param[in] data 待发送的数据包信息
 * @return 是否插入成功
 */
bool FileSender::handleDataSend(SendUnit &unit)
{
    std::lock_guard<std::mutex> lg(tranMutex);

    std::shared_ptr<BaseTransmit> trans = transmits.at(unit.method);
    if(trans.get() != nullptr)
        return trans->startTransmit(unit);

    return false;
}

} //ClientNetwork
