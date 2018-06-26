#ifndef QDB21_WRAPRULE_H
#define QDB21_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class QDB21_WrapRule : public WrapRule
{
public:
    QDB21_WrapRule();

    QByteArray wrap(const ProtocolPackage &package);
    ProtocolPackage unwrap(const QByteArray &data);
};

#endif

#endif // QDB21_WRAPRULE_H
