#ifndef DDS_WRAPRULE_H
#define DDS_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "network/wraprule/wraprule.h"
using namespace ClientNetwork;

class DDS_WrapRule : public WrapRule
{
public:
    DDS_WrapRule();

    QByteArray wrap(const ProtocolPackage &package);
    QByteArray unwrap(const QByteArray &data);
};

#endif

#endif // DDS_WRAPRULE_H
