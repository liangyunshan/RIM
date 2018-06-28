#include "tcp_wraprule.h"

#ifdef __LOCAL_CONTACT__

#include "qdb495_wraprule.h"
#include "qdb21_wraprule.h"
#include "qdb2051_wraprule.h"
#include "rsingleton.h"
using namespace ServerNetwork;

TCP_WrapRule::TCP_WrapRule():WrapRule()
{

}

void TCP_WrapRule::wrap(ProtocolPackage &data)
{
    RSingleton<QDB2051_WrapRule>::instance()->wrap(data);
    RSingleton<QDB21_WrapRule>::instance()->wrap(data);
}

bool TCP_WrapRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    if(!RSingleton<QDB21_WrapRule>::instance()->unwrap(data,result))
        return false;

    if(!RSingleton<QDB2051_WrapRule>::instance()->unwrap(result.data,result))
        return false;

    return true;
}

#endif
