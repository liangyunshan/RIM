#ifndef DATAPACKETRULE_H
#define DATAPACKETRULE_H

#include <QHash>

#include "wraprule.h"
#include "functional"

namespace ClientNetwork{

class DataPacketRule : public WrapRule
{
public:
    DataPacketRule();

    QByteArray wrap(const QByteArray &data);
    bool wrap(const QByteArray & data, std::function<int(const char *,const int)> sendDataFunc);

    QByteArray unwrap(const QByteArray &data);
    bool unwrap(const char * data,const int length,std::function<void(QByteArray &)> recvDataFunc);

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
