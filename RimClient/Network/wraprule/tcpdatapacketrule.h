/*!
 *  @brief     TCP数据包封装
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.26
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef TCPDATAPACKETRULE_H
#define TCPDATAPACKETRULE_H

#include <QHash>

#include "wraprule.h"

namespace ClientNetwork{

class TCPDataPacketRule : public WrapRule
{
public:
    TCPDataPacketRule();

    QByteArray wrap(const ProtocolPackage &data);
    bool wrap(const QByteArray & data, std::function<int(const char *,const int)> sendDataFunc);

    ProtocolPackage unwrap(const QByteArray &data);
    bool unwrap(const char * data,const int length,ByteArrayHandler recvDataFunc);

private:
    void recvData(const char *recvData, int recvLen, QByteArray &result);

private:
    int SendPackId;
    char sendBuff[MAX_SEND_BUFF];

    QByteArray lastRecvBuff;              //断包接收缓冲区
    QHash<int,PacketBuff*> packetBuffs;   //多包缓冲区
};

} // namespace ClientNetwork

#endif // TCPDATAPACKETRULE_H
