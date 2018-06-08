#ifndef UDP_WRAPRULE_H
#define UDP_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class UDP_WrapRule : public WrapRule
{
public:
    UDP_WrapRule();

    QByteArray wrap(const QByteArray &data);
    QByteArray unwrap(const QByteArray &data);
};

#endif

#endif // UDP_WRAPRULE_H
