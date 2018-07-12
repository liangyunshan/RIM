#include "dds_wraprule.h"
#include <QDebug>

#ifdef __LOCAL_CONTACT__

#include "qdb21_wraprule.h"
#include "qdb2051_wraprule.h"
#include "util/rsingleton.h"
using namespace ClientNetwork;


DDS_WrapRule::DDS_WrapRule() : WrapRule()
{

}

QByteArray DDS_WrapRule::wrap(const ProtocolPackage &package)
{
    return QByteArray();
}

QByteArray DDS_WrapRule::unwrap(const QByteArray &data)
{
    return QByteArray();
}

#endif
