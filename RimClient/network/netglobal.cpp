#include "netglobal.h"

QQueue<QByteArray> G_TextRecvBuffs;
QMutex G_TextRecvMutex;
QWaitCondition  G_TextRecvCondition;

QQueue<SendUnit> G_TextSendBuffs;
QMutex G_TextSendMutex;
QWaitCondition  G_TextSendWaitCondition;

QQueue<QByteArray> G_FileRecvBuffs;
QMutex G_FileRecvMutex;
QWaitCondition  G_FileRecvCondition;

QQueue<SendUnit> G_FileSendBuffs;
QMutex G_FileSendMutex;
QWaitCondition  G_FileSendWaitCondition;
