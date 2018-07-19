#ifndef NETGLOBAL_H
#define NETGLOBAL_H

#include "network_global.h"

#include <QByteArray>

#include <mutex>
#include <condition_variable>
#include <queue>

NETWORKSHARED_EXPORT extern std::queue<RecvUnit> G_TextRecvBuffs;
NETWORKSHARED_EXPORT extern std::mutex G_TextRecvMutex;
NETWORKSHARED_EXPORT extern std::condition_variable  G_TextRecvCondition;

NETWORKSHARED_EXPORT extern std::queue<SendUnit> G_TextSendBuffs;
NETWORKSHARED_EXPORT extern std::mutex G_TextSendMutex;
NETWORKSHARED_EXPORT extern std::condition_variable  G_TextSendWaitCondition;

NETWORKSHARED_EXPORT extern std::queue<RecvUnit> G_FileRecvBuffs;
NETWORKSHARED_EXPORT extern std::mutex G_FileRecvMutex;
NETWORKSHARED_EXPORT extern std::condition_variable  G_FileRecvCondition;

NETWORKSHARED_EXPORT extern std::queue<SendUnit> G_FileSendBuffs;
NETWORKSHARED_EXPORT extern std::mutex G_FileSendMutex;
NETWORKSHARED_EXPORT extern std::condition_variable  G_FileSendWaitCondition;

#endif // NETGLOBAL_H
