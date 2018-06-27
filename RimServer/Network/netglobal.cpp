#include "netglobal.h"

std::queue<RecvUnit> G_RecvButts;
std::mutex G_RecvMutex;
std::condition_variable  G_RecvCondition;

std::queue<SendUnit> G_SendButts;
std::mutex G_SendMutex;
std::condition_variable  G_SendCondition;

