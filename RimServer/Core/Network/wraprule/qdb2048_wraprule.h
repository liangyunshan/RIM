#ifndef QDB2048_WRAPRULE_H
#define QDB2048_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ServerNetwork;

class QDB2048_WrapRule : public WrapRule
{
public:
    QDB2048_WrapRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);
};

#endif

#endif // QDB2048_WRAPRULE_H
