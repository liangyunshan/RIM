#ifndef QDB495_WRAPRULE_H
#define QDB495_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ServerNetwork;

class QDB495_WrapRule : public WrapRule
{
public:
    QDB495_WrapRule();

    QByteArray wrap(const QByteArray &data);
    QByteArray unwrap(const QByteArray &data);
};

#endif

#endif // QDB495_WRAPRULE_H
