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

QByteArray TCP_WrapRule::wrap(const ProtocolPackage &data)
{
    ProtocolPackage tempPack = data;

    QByteArray wrapdata = RSingleton<QDB2051_WrapRule>::instance()->wrap(tempPack);
    tempPack.data = wrapdata;
    wrapdata = RSingleton<QDB21_WrapRule>::instance()->wrap(tempPack);
    tempPack.data = wrapdata;
    wrapdata = RSingleton<QDB495_WrapRule>::instance()->wrap(tempPack);
    return wrapdata;
}

ProtocolPackage TCP_WrapRule::unwrap(const QByteArray &data)
{
    ProtocolPackage package;
    package.data = data;

    ProtocolPackage tempPack;
    package = RSingleton<QDB495_WrapRule>::instance()->unwrap(package.data);

    tempPack = RSingleton<QDB21_WrapRule>::instance()->unwrap(package.data);
    package.data = tempPack.data;

    tempPack = RSingleton<QDB2051_WrapRule>::instance()->unwrap(package.data);
    package.data = tempPack.data;

    return package;
}

#endif
