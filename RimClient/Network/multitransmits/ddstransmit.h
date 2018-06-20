#ifndef DDSTRANSMIT_H
#define DDSTRANSMIT_H

#include <memory>

#include "basetransmit.h"
#include "../wraprule/datapacketrule.h"

namespace ClientNetwork{

class NETWORKSHARED_EXPORT DDSTransmit : public BaseTransmit
{
public:
    DDSTransmit();
    ~DDSTransmit();

    bool startTransmit(const SendUnit &unit);
    bool startRecv(char *recvBuff, int recvBuffLen,std::function<void(QByteArray &)> recvDataFunc);

    bool connect(const char *remoteIp, const unsigned short remotePort, int timeouts);
    bool close();


private:
    std::shared_ptr<DataPacketRule> dataPacketRule;
};

}

#endif // DDSTRANSMIT_H
