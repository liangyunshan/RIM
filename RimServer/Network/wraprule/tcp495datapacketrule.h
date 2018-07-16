/*!
 *  @brief     495传输控制协议
 *  @details   采用716传输控制协议报文头，实现TCP传输，解决TCP传输粘包问题
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.26
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef TCP495DATAPACKETRULE_H
#define TCP495DATAPACKETRULE_H

#include "wraprule.h"
#include "../connection/tcpclient.h"

#include <QHash>

#ifdef __LOCAL_CONTACT__

using namespace QDB495;
#define TCP_SEND_BUFF 4096

namespace ServerNetwork{

class IocpContext;
class Handler;

class TCP495DataPacketRule  : public WrapRule
{
public:
    explicit TCP495DataPacketRule();

    void registTextHandler(Handler * dataHandler);
    void bindContext(IocpContext * context,unsigned long recvLen);

    bool wrap(SendUnit &dunit, IocpContextSender sendFunc);
    bool wrap(SendUnit &dunit, ByteSender sendFunc);

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);

private:
    void recvData(const char *recvData, int recvLen);
    int countTotoalIndex(const int totalLength);

private:
    unsigned short SendPackId;
    char sendBuff[TCP_SEND_BUFF];

    IocpContext * ioContext;              /*!< 当前处理的客户端信息 */
    Handler * textHandler;                /*!< 数据处理器 */
    QByteArray lastRecvBuff;              //断包接收缓冲区
    QHash<int,PacketBuff*> packetBuffs;   //多包缓冲区
};

}

#endif

#endif // TCP495DATAPACKETRULE_H
