#ifndef NETGLOBAL_H
#define NETGLOBAL_H

#include "network_global.h"

#include <QQueue>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>

NETWORKSHARED_EXPORT  extern QQueue<QByteArray> G_RecvButts;
NETWORKSHARED_EXPORT  extern QMutex G_RecvMutex;
NETWORKSHARED_EXPORT  extern QWaitCondition  G_RecvCondition;
NETWORKSHARED_EXPORT  extern int TestSocket;

#endif // NETGLOBAL_H
