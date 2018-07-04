#ifndef DATA716PROCESS_H
#define DATA716PROCESS_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class Data716Process
{
public:
    Data716Process();
    void processTextNoAffirm(const ProtocolPackage &data);
    void processTextAffirm(const ProtocolPackage &data);
    void processTextApply(ProtocolPackage &data);

private:
    void processText(const ProtocolPackage &data);
    void applyTextStatus(const ProtocolPackage &data);
};

#endif  //__LOCAL_CONTACT__

#endif // DATA716PROCESS_H
