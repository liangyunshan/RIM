#ifndef QDB21_WRAPRULE_H
#define QDB21_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ServerNetwork;

class QDB21_WrapRule : public WrapRule
{
public:
    QDB21_WrapRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);
};

#endif

#endif // QDB21_WRAPRULE_H
