#include "qdb61a_wraprule.h"

#ifdef __LOCAL_CONTACT__

QDB61A_WrapRule::QDB61A_WrapRule():
    WrapRule()
{

}

QByteArray QDB61A_WrapRule::wrap(const ProtocolPackage &data)
{

    return QByteArray();
}

ProtocolPackage QDB61A_WrapRule::unwrap(const QByteArray &data)
{
    ProtocolPackage package;
    package.data = data;
    return package;
}

#endif
