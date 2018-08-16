#include "qdb495_wraprule.h"
#include <QTime>

#ifdef __LOCAL_CONTACT__

#include "head.h"

using namespace QDB495;
namespace ServerNetwork{

QDB495_WrapRule::QDB495_WrapRule():WrapRule()
{

}

void QDB495_WrapRule::wrap(ProtocolPackage & data)
{
    QDB495_SendPackage SendPackage;
    memset(&SendPackage,0,sizeof(QDB495_SendPackage));

    SendPackage.version = 1;
    SendPackage.packType = WM_DATA_NOAFFIRM;
    SendPackage.packLen = data.data.size();
    SendPackage.offset = 1;
    SendPackage.packAllLen = data.data.size();
    SendPackage.destAddr = data.pack495.destAddr;
    SendPackage.sourceAddr = data.pack495.sourceAddr ;
    SendPackage.serialNo = QTime::currentTime().msecsTo(QTime(0,0,0,0))
                            +SendPackage.destAddr
                            +SendPackage.sourceAddr;


    data.data.prepend((char*)&SendPackage,sizeof(QDB495_SendPackage));
}

bool QDB495_WrapRule::unwrap(const QByteArray & data,ProtocolPackage & result)
{
    if(data.size() < sizeof(QDB495_SendPackage))
        return false;

    QDB495_SendPackage SendPackage;
    memcpy(&SendPackage,data.data(),QDB495_SendPackage_Length);

    result.data = data.right(data.size() - QDB495_SendPackage_Length);
    result.pack495.packType = SendPackage.packType;
    result.pack495.destAddr = SendPackage.destAddr;
    result.pack495.sourceAddr = SendPackage.sourceAddr;

    return true;
}

} //namespace ServerNetwork

#endif
