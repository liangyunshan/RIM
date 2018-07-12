#ifndef QDB61A_WRAPRULE_H
#define QDB61A_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "wraprule.h"

namespace ServerNetwork{

class QDB61A_WrapRule : public WrapRule
{
public:
    QDB61A_WrapRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);
};

} //namespace ServerNetwork

#endif

#endif // QDB61A_WRAPRULE_H
