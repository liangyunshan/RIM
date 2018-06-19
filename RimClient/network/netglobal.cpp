#include "netglobal.h"

std::queue<QByteArray> G_TextRecvBuffs;
std::mutex G_TextRecvMutex;
std::condition_variable  G_TextRecvCondition;

std::queue<SendUnit> G_TextSendBuffs;
std::mutex G_TextSendMutex;
std::condition_variable  G_TextSendWaitCondition;

std::queue<QByteArray> G_FileRecvBuffs;
std::mutex G_FileRecvMutex;
std::condition_variable  G_FileRecvCondition;

std::queue<SendUnit> G_FileSendBuffs;
std::mutex G_FileSendMutex;
std::condition_variable  G_FileSendWaitCondition;
