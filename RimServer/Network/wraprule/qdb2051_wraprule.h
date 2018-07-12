#ifndef QDB2051_WRAPRULE_H
#define QDB2051_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "wraprule.h"
namespace ServerNetwork{

class QDB2051_WrapRule : public WrapRule
{
public:
    QDB2051_WrapRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);
};

}

#endif

#endif // QDB2051_WRAPRULE_H
