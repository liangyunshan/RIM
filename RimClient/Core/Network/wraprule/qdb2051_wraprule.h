#ifndef QDB2051_WRAPRULE_H
#define QDB2051_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class QDB2051_WrapRule : public WrapRule
{
public:
    QDB2051_WrapRule();

    QByteArray wrap(const ProtocolPackage &package);
    QByteArray unwrap(const QByteArray &data);
};

#endif

#endif // QDB2051_WRAPRULE_H
