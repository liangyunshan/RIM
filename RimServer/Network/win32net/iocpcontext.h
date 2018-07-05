/*!
 *  @brief     重叠I/O结构
 *  @details   记录客户端连接信息，为每个客户端分配缓冲区。
 *  @file      iocpcontext.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.13
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef IOEVENT_H
#define IOEVENT_H

#include "../head.h"
#include "../network_global.h"
#include "../connection/tcpclient.h"
#include "iopacket.h"

namespace ServerNetwork {

/*!
 *  @brief ICOP请求类型
 */
enum IocpType
{
    IOCP_ACCPET = 0,        /*!< Socket连接请求*/
    IOCP_RECV,              /*!< Socket信息接收*/
    IOCP_SEND,              /*!< Socket信息发送*/
};

class NETWORKSHARED_EXPORT IocpContext
{
public:
    IocpContext();

    static IocpContext * create(IocpType type,TcpClient * client);
    static void destory(IocpContext * context);

    void setSocket(SOCKET sock){m_sock = sock;}

    void setClient(TcpClient * client){m_client = client;}
    TcpClient * getClient(){return m_client;}

    void setType(int type){m_type = type;}
    int getType(){return m_type;}

    char * getPakcet(){return dataBuff;}
    WSAOVERLAPPED & getOverLapped(){return overlapped;}
    WSABUF& getWSABUF(){return wsabuffer;}

private:
    WSAOVERLAPPED overlapped;
    int m_type;                                //当前的请求的类型;
    int m_sock;                                //处理请求的socket
    TcpClient * m_client;
    WSABUF wsabuffer;
    char dataBuff[MAX_RECV_SIZE];
};

}// namespace ServerNetwork

#endif // IOEVENT_H
