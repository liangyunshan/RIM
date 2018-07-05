/*!
 *  @brief     TCP数据包封装
 *  @details   用于对TCP数据进行解包，确保数据接收正确。
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
    explicit TCPDataPacketRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);

    bool wrap(const ProtocolPackage &data, std::function<int(const char *,const int)> sendDataFunc);
    bool unwrap(const char * data,const int length,DataHandler handler);

private:
    bool recvData(const char *recvData, int recvLen);

private:
    int SendPackId;
    char sendBuff[MAX_SEND_BUFF];

    QByteArray lastRecvBuff;              //断包接收缓冲区
    QHash<int,PacketBuff*> packetBuffs;   //多包缓冲区

    DataHandler dHandler;
};

} // namespace ClientNetwork

#endif // TCPDATAPACKETRULE_H
