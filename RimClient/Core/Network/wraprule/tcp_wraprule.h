#ifndef TCP_WRAPRULE_H
#define TCP_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class TCP_WrapRule : public WrapRule
{
public:
    TCP_WrapRule();

    QByteArray wrap(const ProtocolPackage &package);
    ProtocolPackage unwrap(const QByteArray &data);
};

#endif

#endif // TCP_WRAPRULE_H
