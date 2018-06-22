#include "dds_wraprule.h"
#include <QDebug>

#ifdef __LOCAL_CONTACT__

#include "qdb495_wraprule.h"
#include "qdb21_wraprule.h"
#include "qdb2051_wraprule.h"
#include "rsingleton.h"
using namespace ClientNetwork;


DDS_WrapRule::DDS_WrapRule() : WrapRule()
{

}

QByteArray DDS_WrapRule::wrap(const ProtocolPackage &package)
{
    ProtocolPackage tempPack = package;
    QByteArray wrapdata = RSingleton<QDB2051_WrapRule>::instance()->wrap(tempPack);
    tempPack.cFileData = wrapdata;
    wrapdata = RSingleton<QDB21_WrapRule>::instance()->wrap(tempPack);
    tempPack.cFileData = wrapdata;
    wrapdata = RSingleton<QDB495_WrapRule>::instance()->wrap(tempPack);
    return wrapdata;
}

QByteArray DDS_WrapRule::unwrap(const QByteArray &data)
{
    QByteArray wrapdata = RSingleton<QDB495_WrapRule>::instance()->unwrap(data);
    wrapdata = RSingleton<QDB21_WrapRule>::instance()->unwrap(wrapdata);
    wrapdata = RSingleton<QDB2051_WrapRule>::instance()->unwrap(wrapdata);
    return wrapdata;
}

#endif
