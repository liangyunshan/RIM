#ifndef NETGLOBAL_H
#define NETGLOBAL_H

#include "network_global.h"
#include "head.h"

#include <QQueue>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>

NETWORKSHARED_EXPORT  extern QQueue<SocketInData> G_RecvButts;
NETWORKSHARED_EXPORT  extern QMutex G_RecvMutex;
NETWORKSHARED_EXPORT  extern QWaitCondition  G_RecvCondition;

NETWORKSHARED_EXPORT  extern QQueue<SocketOutData> G_SendButts;
NETWORKSHARED_EXPORT  extern QMutex G_SendMutex;
NETWORKSHARED_EXPORT  extern QWaitCondition  G_SendCondition;

#endif // NETGLOBAL_H
