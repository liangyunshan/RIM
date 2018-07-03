#ifndef DATA716PROCESS_H
#define DATA716PROCESS_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class Data716Process
{
public:
    Data716Process();
    void processTextNoAffirm(ProtocolPackage &data);
    void processTextAffirm(ProtocolPackage &data);
    void processTextApply(ProtocolPackage &data);

private:
    void processText(ProtocolPackage &data);
    void ApplyTextStatus();
};

#endif  //__LOCAL_CONTACT__

#endif // DATA716PROCESS_H
