/*!
 *  @brief     TCP数据封包解包
 *  @details   此文件属于网络层： \n
 *             1.数据发送：将应用层数据按照分包的大小，分别加入网络层传输头，确保数据传输正常 \n
 *             2.数据接收：网络层接收数据后，对数据进行组包，待接收到完整数据包后，去除包头，将数据体交由应用层处理. \n
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.20
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATAPACKETRULE_H
#define DATAPACKETRULE_H

#include <QHash>

#include "wraprule.h"

namespace ClientNetwork{

class DataPacketRule : public WrapRule
{
public:
    DataPacketRule();

    QByteArray wrap(const QByteArray &data);
    bool wrap(const QByteArray & data, std::function<int(const char *,const int)> sendDataFunc);

    QByteArray unwrap(const QByteArray &data);
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

#endif // DATAPACKETRULE_H
