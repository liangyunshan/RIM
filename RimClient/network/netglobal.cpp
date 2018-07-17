#include "netglobal.h"

std::queue<RecvUnit> G_TextRecvBuffs;
std::mutex G_TextRecvMutex;
std::condition_variable  G_TextRecvCondition;

std::queue<SendUnit> G_TextSendBuffs;
std::mutex G_TextSendMutex;
std::condition_variable  G_TextSendWaitCondition;

std::queue<RecvUnit> G_FileRecvBuffs;
std::mutex G_FileRecvMutex;
std::condition_variable  G_FileRecvCondition;

std::queue<SendUnit> G_FileSendBuffs;
std::mutex G_FileSendMutex;
std::condition_variable  G_FileSendWaitCondition;

static unsigned int SERIALNO_STATIC = 1;
static unsigned int SERIALNO_MAX_STATIC = 65535;

std::mutex SerialNoMutex;
unsigned int FrashSerialNo()
{
    SerialNoMutex.lock();
    SERIALNO_STATIC>=SERIALNO_MAX_STATIC?(SERIALNO_STATIC=1) : (SERIALNO_MAX_STATIC++);
    SerialNoMutex.unlock();
    return SERIALNO_STATIC;
}

unsigned int SetSerialNo(unsigned int No)
{
    SerialNoMutex.lock();
    SERIALNO_STATIC = No;
    SerialNoMutex.unlock();
    return SERIALNO_STATIC;
}

