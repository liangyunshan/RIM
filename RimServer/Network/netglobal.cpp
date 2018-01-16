#include "netglobal.h"

QQueue<QByteArray> G_RecvButts;

QMutex G_RecvMutex;
QWaitCondition  G_RecvCondition;

int TestSocket;
