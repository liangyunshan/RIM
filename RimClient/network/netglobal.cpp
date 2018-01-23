#include "netglobal.h"

QQueue<QByteArray> G_RecvButts;
QMutex G_RecvMutex;
QWaitCondition  G_RecvCondition;

QQueue<QByteArray> G_SendBuff;
QMutex G_SendMutex;
QWaitCondition  G_SendWaitCondition;
