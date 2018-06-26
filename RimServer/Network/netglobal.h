#ifndef NETGLOBAL_H
#define NETGLOBAL_H

#include "network_global.h"
#include "head.h"

#include <queue>
#include <condition_variable>
#include <mutex>

NETWORKSHARED_EXPORT  extern std::queue<SocketInData> G_RecvButts;
NETWORKSHARED_EXPORT  extern std::mutex G_RecvMutex;
NETWORKSHARED_EXPORT  extern std::condition_variable  G_RecvCondition;

NETWORKSHARED_EXPORT  extern std::queue<SocketOutData> G_SendButts;
NETWORKSHARED_EXPORT  extern std::mutex G_SendMutex;
NETWORKSHARED_EXPORT  extern std::condition_variable  G_SendCondition;

#endif // NETGLOBAL_H
