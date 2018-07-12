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

    SendPackage.bVersion = 1;
    SendPackage.bPackType = WM_DATA_NOAFFIRM;
    SendPackage.wPackLen = data.data.size();
    SendPackage.wOffset = 1;
    SendPackage.dwPackAllLen = data.data.size();
    SendPackage.wDestAddr = data.wDestAddr;
    SendPackage.wSourceAddr = data.wSourceAddr ;
    SendPackage.wSerialNo = QTime::currentTime().msecsTo(QTime(0,0,0,0))
                            +SendPackage.wDestAddr
                            +SendPackage.wSourceAddr;


    data.data.prepend((char*)&SendPackage,sizeof(QDB495_SendPackage));
}

bool QDB495_WrapRule::unwrap(const QByteArray & data,ProtocolPackage & result)
{
    if(data.size() < sizeof(QDB495_SendPackage))
        return false;

    QDB495_SendPackage SendPackage;
    memcpy(&SendPackage,data.data(),QDB495_SendPackage_Length);

    result.data = data.right(data.size() - QDB495_SendPackage_Length);
    result.bPackType = SendPackage.bPackType;
    result.wDestAddr = SendPackage.wDestAddr;
    result.wSourceAddr = SendPackage.wSourceAddr;

    return true;
}

} //namespace ServerNetwork

#endif
