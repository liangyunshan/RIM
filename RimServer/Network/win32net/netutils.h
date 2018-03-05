/*!
 *  @brief     网络操作工具
 *  @details   可用于异步监听、创建I/O接收、创建I/O发送等
 *  @file      netutils.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.12
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef NETUTILS_H
#define NETUTILS_H

#include "SharedIocpData.h"
#include "../netglobal.h"

namespace ServerNetwork {
namespace NetUtil {

    NETWORKSHARED_EXPORT void postRecv(IocpContext *ioData);

    NETWORKSHARED_EXPORT void postAccept(SharedIocpData * server);

    NETWORKSHARED_EXPORT bool crateIocp(SOCKET clientSock, SharedIocpData *sharedData, DWORD iocpKey);

    NETWORKSHARED_EXPORT bool send(SOCKET dest,const char * data,int length);


}   //namespace NetUtil
}   //namespace ServerNetwork

#endif // NETUTILS_H
