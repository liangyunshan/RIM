#ifndef NETGLOBAL_H
#define NETGLOBAL_H

#include "network_global.h"

#include <QQueue>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>

NETWORKSHARED_EXPORT extern QQueue<QByteArray> G_TextRecvBuffs;
NETWORKSHARED_EXPORT extern QMutex G_TextRecvMutex;
NETWORKSHARED_EXPORT extern QWaitCondition  G_TextRecvCondition;

NETWORKSHARED_EXPORT extern QQueue<SendUnit> G_TextSendBuffs;
NETWORKSHARED_EXPORT extern QMutex G_TextSendMutex;
NETWORKSHARED_EXPORT extern QWaitCondition  G_TextSendWaitCondition;

NETWORKSHARED_EXPORT extern QQueue<QByteArray> G_FileRecvBuffs;
NETWORKSHARED_EXPORT extern QMutex G_FileRecvMutex;
NETWORKSHARED_EXPORT extern QWaitCondition  G_FileRecvCondition;

NETWORKSHARED_EXPORT extern QQueue<SendUnit> G_FileSendBuffs;
NETWORKSHARED_EXPORT extern QMutex G_FileSendMutex;
NETWORKSHARED_EXPORT extern QWaitCondition  G_FileSendWaitCondition;


#endif // NETGLOBAL_H
