#ifndef QDB61A_WRAPRULE_H
#define QDB61A_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class QDB61A_WrapRule : public WrapRule
{
public:
    QDB61A_WrapRule();

    QByteArray wrap(const ProtocolPackage &data);
    QByteArray unwrap(const QByteArray &data);
};

#endif

#endif // QDB61A_WRAPRULE_H
