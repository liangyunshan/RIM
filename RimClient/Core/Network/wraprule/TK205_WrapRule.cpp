#include "tk205_wraprule.h"

#include "rsingleton.h"

#ifdef __LOCAL_CONTACT__

TK205_WrapRule::TK205_WrapRule():
    WrapRule()
{

}

QByteArray TK205_WrapRule::wrap(const QByteArray &data)
{

    return QByteArray();
}

QByteArray TK205_WrapRule::unwrap(const QByteArray &data)
{
    return QByteArray();
}

#endif

