#ifndef QDB495_WRAPRULE_H
#define QDB495_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class QDB495_WrapRule : public WrapRule
{
public:
    QDB495_WrapRule();

    QByteArray wrap(const ProtocolPackage &package);
    ProtocolPackage unwrap(const QByteArray &data);
};

#endif

#endif // QDB495_WRAPRULE_H
