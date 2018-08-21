#include "tcptransmit.h"

#include "Util/rlog.h"
#include "../rsocket.h"

#ifdef Q_OS_WIN
#include <WinSock2.h>
#endif

namespace ClientNetwork{

TcpTransmit::TcpTransmit():
    BaseTransmit(),tcpSocket(nullptr)
{
#ifdef __LOCAL_CONTACT__
    dataPacketRule = std::make_shared<TCP495DataPacketRule>();
#else
    dataPacketRule = std::make_shared<TCPDataPacketRule>();
#endif
    tcpSocket = new RSocket();

    sendFunc = [&](const char * buff,const int length)->int{return tcpSocket->send(buff,length);};
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
 * @note 若传输失败，则直接关闭socket，并将错误信息交由上层处理。
 */
bool TcpTransmit::startTransmit(SendUnit &unit, SendCallbackFunc func)
{
    if(tcpSocket && tcpSocket->isValid()){
        if(dataPacketRule->wrap(unit.dataUnit,sendFunc)){
            if(func != nullptr){
                FileDataSendProgress progress;
                progress.wSerialNo = unit.dataUnit.usSerialNo;
                progress.wDestAddr = unit.dataUnit.wDestAddr;
                progress.dwPackAllLen = unit.dataUnit.dwPackAllLen;
                progress.wPackLen = unit.dataUnit.data.length();
                func(progress);
            }
            return true;
        }
    }

    close();
    RLOG_ERROR("Tcp send a data error!");
    return false;
}

/*!
 * @brief 开始TCP数据接收
 * @param[in] recvBuff 接收数据缓存区
 * @param[in] recvBuffLen 最大接收长度
 * @param[in] recvDataFunc 接收解析后处理函数
 * @return 是否接收处理成功
 */
bool TcpTransmit::startRecv(char *recvBuff, int recvBuffLen, DataHandler recvDataFunc)
{
    int recvLen = tcpSocket->recv(recvBuff,recvBuffLen);
    if(recvLen > 0)
    {
        if(!dataPacketRule->unwrap(recvBuff,recvLen,recvDataFunc)){
            RLOG_ERROR("Tcp socket parse error! %d",tcpSocket->getLastError());
        }else{
            return true;
        }
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
