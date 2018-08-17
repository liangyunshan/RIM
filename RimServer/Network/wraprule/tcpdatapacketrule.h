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
#include "../connection/tcpclient.h"

namespace ServerNetwork{
class IocpContext;
class Handler;

class TCPDataPacketRule : public WrapRule
{
public:
    explicit TCPDataPacketRule();

    bool wrap(const SendUnit &data, IocpContextSender sendFunc);
    bool wrap(const SendUnit &dunit, ByteSender sendFunc){return true;}

    void registDataHandler(Handler * dataHandler);
    void bindContext(IocpContext * context,unsigned long recvLen);

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);

private:
    void recvData(const char *recvData, int recvLen);

private:
    int SendPackId;
    char sendBuff[MAX_SEND_BUFF];

    IocpContext * ioContext;              /*!< 当前处理的客户端信息 */
    Handler * handler;                    /*!< 数据处理器 */
    QByteArray lastRecvBuff;              //断包接收缓冲区
    QHash<int,PacketBuff*> packetBuffs;   //多包缓冲区
};

} // namespace ClientNetwork

#endif // TCPDATAPACKETRULE_H
