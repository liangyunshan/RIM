#include "tcptransmit.h"

#include "Util/rlog.h"
#include "../socket.h"
#include "../win32net/iocpcontext.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <WinSock2.h>
#endif

namespace ServerNetwork{

TcpTransmit::TcpTransmit():
    BaseTransmit(),tcpSocket(nullptr)
{
#ifdef __LOCAL_CONTACT__
    dataPacketRule = std::make_shared<TCP495DataPacketRule>();
#else
    dataPacketRule = std::make_shared<TCPDataPacketRule>();
#endif
//    tcpSocket = new RSocket();

    sendFunc = std::bind(&TcpTransmit::sendIocpData,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
}

TcpTransmit::~TcpTransmit()
{
    dataPacketRule.reset();
}

CommMethod TcpTransmit::type()
{
    return C_TCP;
}

QString TcpTransmit::name()
{
    return "TCP";
}

/*!
 * @brief 开始传输指定的数据单元
 * @param[in] SendUnit 待发送的数据单元
 * @return 是否传输成功
 * @note 1.根据发送的目的服务器是否与当前服务器一致，若一致则走IOCP链路；若不一致，则走普通链路。
 *       2.若传输失败，则直接关闭socket，并将错误信息交由上层处理。
 */
bool TcpTransmit::startTransmit(const SendUnit &unit)
{
    if(unit.localServer){
        if(dataPacketRule->wrap(unit,sendFunc))
            return true;
    }else{
        if(dataPacketRule->wrap(unit,byteSendFunc))
            return true;
    }

    RLOG_ERROR("Tcp send a data error!");
    return false;
}

/*!
 * @brief 通过IOCP发送数据
 * @param[in] sockId 目标主机连接sock id
 * @param[in] context 重叠I/O，保存待发送数据信息
 * @param[in] sendLength 实际发送数据长度
 * @return 是否发送成功
 */
bool TcpTransmit::sendIocpData(int sockId,IocpContext * context,DWORD & sendLength)
{
    DWORD sendFlags = 0;

    if(WSASend(sockId, &context->getWSABUF(), 1, &sendLength, sendFlags, &context->getOverLapped(), NULL) == SOCKET_ERROR)
    {
        int error = WSAGetLastError();
        if(error != ERROR_IO_PENDING){
            //TODO 对错误进行处理
            return false;
        }
    }
    return true;
}

/*!
 * @brief 通过普通socket发送数据
 * @note 若发送失败，上层应用应该尽快关闭当前socket。
 * @param[in] sockId 目标主机连接sock id
 * @param[in] buff  待发送数据缓冲区
 * @param[in] length 待发送数据长度
 * @return 是否发送成功
 */
int TcpTransmit::sendByteData(int sockId,const char * buff,const int length)
{
    if(buff == NULL)
        return -1;

    int sendLen = 0;
    while(sendLen != length)
    {
        int ret = ::send(sockId,buff+sendLen,length-sendLen,0);
        if (ret <= 0){
            sendLen = -1;
            break;
        }
        sendLen += ret;
    }
    return sendLen;
}

bool TcpTransmit::startRecv(char *recvBuff, int recvBuffLen, ByteArrayHandler recvDataFunc)
{
    int recvLen = tcpSocket->recv(recvBuff,recvBuffLen);
    if(recvLen > 0)
    {
//        if(!dataPacketRule->unwrap(recvBuff,recvLen,recvDataFunc)){
//            RLOG_ERROR("Tcp socket parse error! %d",tcpSocket->getLastError());
//        }else{
//            return true;
//        }
    }
    else if(recvLen == 0)
    {
        RLOG_ERROR("Tcp socket closed! %d",tcpSocket->getLastError());
    }else{
        int error = tcpSocket->getLastError();
        if(errno == EAGAIN || errno == EWOULDBLOCK || error == (int)WSAETIMEDOUT){
            return true;
        }
        RLOG_ERROR("Tcp socket occour error! %d",tcpSocket->getLastError());
    }

    close();

    return false;
}

bool TcpTransmit::close()
{
    if(!tcpSocket)
        return false;

    if(netConnected && tcpSocket->isValid()){
        if(tcpSocket->closeSocket()){
            netConnected = false;
            return true;
        }
    }
    return false;
}

/*!
 * @brief 连接远程网络地址
 * @param[in] remoteIp 远程IP地址
 * @param[in] remotePort 远程端口号
 * @param[in] timeouts 连接超时时间
 * @return 是否连接成功
 */
bool TcpTransmit::connect(const char *remoteIp, const unsigned short remotePort, int timeouts)
{
    if(!tcpSocket || !remoteIp || remotePort <= 0)
        return false;

    if(!netConnected){

        if(!tcpSocket->isValid() && tcpSocket->createSocket())
        {
            int timeout = 3000;
            tcpSocket->setSockopt(SO_RCVTIMEO,(char *)&timeout,sizeof(timeout));

            if(!netConnected && tcpSocket->isValid() && tcpSocket->connect(remoteIp,remotePort,timeouts)){
                netConnected = true;
            }else{
                tcpSocket->closeSocket();
            }
        }else{
            tcpSocket->closeSocket();
            return false;
        }
    }
    return netConnected;
}

}
