/*!
 *  @brief     TCP传输模式
 *  @details   使用TCP方式封装了数据传输格式协议
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef TCPTRANSMIT_H
#define TCPTRANSMIT_H

#include <memory>

#include "basetransmit.h"
#include "../wraprule/tcpdatapacketrule.h"
#include "../wraprule/tcp495datapacketrule.h"


namespace ClientNetwork{

class RSocket;

class NETWORKSHARED_EXPORT TcpTransmit : public BaseTransmit
{
public:
    TcpTransmit();
    ~TcpTransmit();

    CommMethod type();
    QString name();

    bool startTransmit(SendUnit &unit);
    bool startRecv(char *recvBuff, int recvBuffLen,DataHandler recvDataFunc);

    bool connect(const char *remoteIp, const unsigned short remotePort, int timeouts);
    bool close();

private:
#ifdef __LOCAL_CONTACT__
    std::shared_ptr<TCP495DataPacketRule> dataPacketRule;
#else
    std::shared_ptr<TCPDataPacketRule> dataPacketRule;
#endif
    RSocket* tcpSocket;
};

}

#endif // TCPTRANSMIT_H
