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
NETWORKSHARED_EXPORT extern unsigned int FrashSerialNo();
NETWORKSHARED_EXPORT extern unsigned int SetSerialNo(unsigned int No);

#define SERIALNO_FRASH FrashSerialNo()
#define SERIALNO_SetFrash(No) SetSerialNo(unsigned int No)

#endif // NETGLOBAL_H
