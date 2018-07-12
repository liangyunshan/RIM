#ifndef QDB2048_WRAPRULE_H
#define QDB2048_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "wraprule.h"

namespace ServerNetwork{

class QDB2048_WrapRule : public WrapRule
{
public:
    QDB2048_WrapRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);
};

} //namespace ServerNetwork

#endif

#endif // QDB2048_WRAPRULE_H
