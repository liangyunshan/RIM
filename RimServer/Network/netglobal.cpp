#include "netglobal.h"

QQueue<SocketInData> G_RecvButts;
QMutex G_RecvMutex;
QWaitCondition  G_RecvCondition;

QQueue<SocketOutData> G_SendButts;
QMutex G_SendMutex;
QWaitCondition  G_SendCondition;

