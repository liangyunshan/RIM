#ifndef QDB21_WRAPRULE_H
#define QDB21_WRAPRULE_H

#ifdef __LOCAL_CONTACT__

#include "wraprule.h"

namespace ServerNetwork{

class QDB21_WrapRule : public WrapRule
{
public:
    QDB21_WrapRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);

private:
    void wrapTime(char * output, int intput, int length);
    void unwrapTime(int &output, char * input, int length);
};

} //ServerNetwork

#endif

#endif // QDB21_WRAPRULE_H
