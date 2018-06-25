#ifndef TCP_WRAPRULE_H
#define TCP_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ServerNetwork;

class TCP_WrapRule : public WrapRule
{
public:
    TCP_WrapRule();

    QByteArray wrap(const QByteArray &data);
    QByteArray unwrap(const QByteArray &data);
};

#endif

#endif // TCP_WRAPRULE_H
