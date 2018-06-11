#include "tcptransmit.h"

#include "Util/rlog.h"
#include "../rsocket.h"

#ifdef Q_OS_WIN
#include <WinSock2.h>
#endif

namespace ClientNetwork{

TcpTransmit::TcpTransmit():
    BaseTransmit(C_TCP),tcpSocket(nullptr)
{
    dataPacketRule = std::make_shared<DataPacketRule>();

    tcpSocket = new RSocket();
}

TcpTransmit::~TcpTransmit()
{
    dataPacketRule.reset();
}

bool TcpTransmit::startTransmit(const SendUnit &unit)
{
    if(tcpSocket && tcpSocket->isValid()){
        auto func = [&](const char * buff,const int length)->int{
            return tcpSocket->send(buff,length);
        };

        if(dataPacketRule->wrap(unit.data,func))
            return true;
    }

    RLOG_ERROR("Tcp send a data error!");
    if(tcpSocket)
        tcpSocket->closeSocket();

    return false;
}

bool TcpTransmit::startRecv(char *recvBuff, int recvBuffLen, std::function<void(QByteArray &)> recvDataFunc)
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

    tcpSocket->closeSocket();

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
    return true;
}

}
