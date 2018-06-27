#include "sockdatahandler.h"

#include "netglobal.h"

namespace ServerNetwork{

SockDataHandler::SockDataHandler()
{

}

void SockDataHandler::handle(const RecvUnit &recvData)
{
      std::unique_lock<std::mutex> ul(G_RecvMutex);
      G_RecvButts.push(recvData);

      G_RecvCondition.notify_one();
}

}
