#include "tcp_wraprule.h"

#ifdef __LOCAL_CONTACT__

#include "qdb495_wraprule.h"
#include "qdb21_wraprule.h"
#include "rsingleton.h"

TCP_WrapRule::TCP_WrapRule():WrapRule()
{

}

QByteArray TCP_WrapRule::wrap(const QByteArray &data)
{
    return RSingleton<QDB21_WrapRule>::instance()->wrap(RSingleton<QDB495_WrapRule>::instance()->wrap(data));
}

QByteArray TCP_WrapRule::unwrap(const QByteArray &data)
{
    return QByteArray();
}

#endif
