#ifndef QDB2051_WRAPRULE_H
#define QDB2051_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ServerNetwork;

class QDB2051_WrapRule : public WrapRule
{
public:
    QDB2051_WrapRule();

    QByteArray wrap(const QByteArray &data);
    QByteArray unwrap(const QByteArray &data);
};

#endif

#endif // QDB2051_WRAPRULE_H
