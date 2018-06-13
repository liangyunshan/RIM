#include "qdb495_wraprule.h"

#ifdef __LOCAL_CONTACT__

QDB495_WrapRule::QDB495_WrapRule():WrapRule()
{

}

QByteArray QDB495_WrapRule::wrap(const QByteArray &data)
{

    return QByteArray(data);
}

QByteArray QDB495_WrapRule::unwrap(const QByteArray &data)
{
    return QByteArray(data);
}

#endif
