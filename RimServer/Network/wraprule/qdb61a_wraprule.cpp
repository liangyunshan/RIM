#include "qdb61a_wraprule.h"

#ifdef __LOCAL_CONTACT__

namespace ServerNetwork{

QDB61A_WrapRule::QDB61A_WrapRule():
    WrapRule()
{

}

void QDB61A_WrapRule::wrap(ProtocolPackage &data)
{
    Q_UNUSED(data)
}

bool QDB61A_WrapRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    Q_UNUSED(data)
    Q_UNUSED(result)
    return false;
}
} //namespace ServerNetwork

#endif
