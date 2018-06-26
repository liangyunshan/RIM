#ifndef UDP_WRAPRULE_H
#define UDP_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ServerNetwork;

class UDP_WrapRule : public WrapRule
{
public:
    UDP_WrapRule();

    QByteArray wrap(const ProtocolPackage &package);
    ProtocolPackage unwrap(const QByteArray &data);
};

#endif

#endif // UDP_WRAPRULE_H
