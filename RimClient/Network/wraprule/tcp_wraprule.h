#ifndef TCP_WRAPRULE_H
#define TCP_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "wraprule.h"
namespace ClientNetwork{

class NETWORKSHARED_EXPORT TCP_WrapRule : public WrapRule
{
public:
    TCP_WrapRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);
};

} //namespace ClientNetwork

#endif

#endif // TCP_WRAPRULE_H
