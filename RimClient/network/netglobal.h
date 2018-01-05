#ifndef NETGLOBAL_H
#define NETGLOBAL_H

#include <QQueue>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>

extern QQueue<QByteArray> G_RecvButts;

extern QMutex G_RecvMutex;
extern QWaitCondition  G_RecvCondition;


#endif // NETGLOBAL_H
