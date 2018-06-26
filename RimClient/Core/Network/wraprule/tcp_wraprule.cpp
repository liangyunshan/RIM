#include "tcp_wraprule.h"

#ifdef __LOCAL_CONTACT__

#include "qdb495_wraprule.h"
#include "qdb21_wraprule.h"
#include "qdb2051_wraprule.h"
#include "rsingleton.h"
#include <QByteArray>
using namespace ClientNetwork;

TCP_WrapRule::TCP_WrapRule():WrapRule()
{

}

QByteArray TCP_WrapRule::wrap(const ProtocolPackage &package)
{
    ProtocolPackage tempPack = package;

    QByteArray wrapdata = RSingleton<QDB2051_WrapRule>::instance()->wrap(tempPack);
    tempPack.cFileData = wrapdata;
    wrapdata = RSingleton<QDB21_WrapRule>::instance()->wrap(tempPack);
    tempPack.cFileData = wrapdata;
    wrapdata = RSingleton<QDB495_WrapRule>::instance()->wrap(tempPack);
    return wrapdata;
}

ProtocolPackage TCP_WrapRule::unwrap(const QByteArray &data)
{
    ProtocolPackage package;
    package.cFileData = data;

    ProtocolPackage tempPack;
    package = RSingleton<QDB495_WrapRule>::instance()->unwrap(package.cFileData);

    tempPack = RSingleton<QDB21_WrapRule>::instance()->unwrap(package.cFileData);
    package.cFileData = tempPack.cFileData;

    tempPack = RSingleton<QDB2051_WrapRule>::instance()->unwrap(package.cFileData);
    package.cFileData = tempPack.cFileData;

    return package;
}

#endif
