#include "tk205_wraprule.h"

#include "Util/rsingleton.h"

#ifdef __LOCAL_CONTACT__
namespace ServerNetwork{

TK205_WrapRule::TK205_WrapRule():
    WrapRule()
{

}

void TK205_WrapRule::wrap(ProtocolPackage &data)
{
    Q_UNUSED(data)
}

bool TK205_WrapRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    Q_UNUSED(data)
    Q_UNUSED(result)
    return false;
}

} //namespace ServerNetwork

#endif

