#include "sockdatahandler.h"

#include "netglobal.h"

namespace ServerNetwork{

SockTextDataHandler::SockTextDataHandler()
{

}

void SockTextDataHandler::handle(const RecvUnit &recvData)
{
      std::unique_lock<std::mutex> ul(G_RecvMutex);
      G_RecvButts.push(recvData);

      G_RecvCondition.notify_one();
}

}
