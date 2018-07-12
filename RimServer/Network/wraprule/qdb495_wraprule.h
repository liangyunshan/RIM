#ifndef QDB495_WRAPRULE_H
#define QDB495_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "wraprule.h"

namespace ServerNetwork{

class QDB495_WrapRule : public WrapRule
{
public:
    QDB495_WrapRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);
};

} //namespace ServerNetwork

#endif

#endif // QDB495_WRAPRULE_H
