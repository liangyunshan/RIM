#include "netglobal.h"

std::queue<SendUnit> G_TextSendBuffs;
std::mutex G_TextSendMutex;
std::condition_variable  G_TextSendWaitCondition;

std::queue<SendUnit> G_FileSendBuffs;
std::mutex G_FileSendMutex;
std::condition_variable  G_FileSendWaitCondition;
