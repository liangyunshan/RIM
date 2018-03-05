/*!
 *  @brief     监听信息
 *  @details   保存服务器监听的信息，以供各个线程共享调用
 *  @file      iocpdata.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SHAREDIOCPDATA_H
#define SHAREDIOCPDATA_H

#include "../head.h"
#include "iocpcontext.h"
#include "../socket.h"

namespace ServerNetwork {

class TcpClientManager;

class SharedIocpData
{
public:
    SharedIocpData();

    void crateIocpContext();

    IocpContext * m_iocpContext;                /*!< 服务器端I/O重叠结构 */

    RSocket m_listenSock;                       /*!< 服务端接听端口 */
    HANDLE m_ioCompletionPort;                  /*!< iocp端口 */
    TcpClientManager * m_clientManager;         /*!< 客户端管理 */
};

}//namespace ServerNetwork

#endif // IOCPDATA_H
