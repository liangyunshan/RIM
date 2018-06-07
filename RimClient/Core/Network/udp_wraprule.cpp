#include "udp_wraprule.h"

#ifdef __LOCAL_CONTACT__

#include "qdb61a_wraprule.h"
#include "tk205_wraprule.h"
#include "rsingleton.h"

UDP_WrapRule::UDP_WrapRule():WrapRule()
{

}

QByteArray UDP_WrapRule::wrap(const QByteArray &data)
{
    return RSingleton<QDB61A_WrapRule>::instance()->wrap(RSingleton<TK205_WrapRule>::instance()->wrap(data));
}

QByteArray UDP_WrapRule::unwrap(const QByteArray &data)
{
    return QByteArray();
}

#endif
