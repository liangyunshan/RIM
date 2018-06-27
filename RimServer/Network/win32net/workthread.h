/*!
 *  @brief     IOCP接收处理线程
 *  @details   创建处理器数量*2的处理线程，接收网络I/O处理，接收完成后，将数据放入待处理队列
 *  @file      workthread.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.12
 *  @warning   【1】接收数据时，需要考虑被拆包后导致不同线程接收数据的各个部分，造成数据不完整；
 *             【2】退出时应通知处理线程退出
 *  @copyright NanJing RenGu.
 *  @note   20180123:wey:增加分包接收、组包功能；增加分包发送功能；
 */
#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include "SharedIocpData.h"

#include <memory>

#include "../wraprule/tcpdatapacketrule.h"

#ifdef __LOCAL_CONTACT__
#include "../wraprule/tcp495datapacketrule.h"
#endif

namespace ServerNetwork {

class TcpClient;

class WorkThread
{
public:
    WorkThread(SharedIocpData * data);

    friend unsigned  __stdcall iocpProc(LPVOID v);

private:
    void handleIo(IocpContext *ioData, unsigned long recvLength, TcpClient *recvClient);
    void handleRecv(IocpContext *ioData, unsigned long recvLen, TcpClient* tcpClient);
    void handleAccept(IocpContext *ioData);
    void handleSend(IocpContext *ioData);
    void handleClose(IocpContext *ioData);

private:
    HANDLE threadId;
    SharedIocpData * serverSharedData;

#ifdef __LOCAL_CONTACT__
    std::shared_ptr<TCP495DataPacketRule> dataPacketRule;
#else
    std::shared_ptr<TCPDataPacketRule> dataPacketRule;
#endif

};

}   //namespace ServerNetwork

#endif // WORKTHREAD_H
