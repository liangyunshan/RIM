#include "qdb61a_wraprule.h"

#ifdef __LOCAL_CONTACT__

QDB61A_WrapRule::QDB61A_WrapRule():
    WrapRule()
{

}

void QDB61A_WrapRule::wrap(ProtocolPackage &data)
{

}

bool QDB61A_WrapRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    return false;
}


#endif
