#include "tk205_wraprule.h"

#include "util/rsingleton.h"

#ifdef __LOCAL_CONTACT__

TK205_WrapRule::TK205_WrapRule():
    WrapRule()
{

}

void TK205_WrapRule::wrap(ProtocolPackage &data)
{

}

bool TK205_WrapRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    return false;
}


#endif

