#include "qdb495_wraprule.h"
#include <QTime>


#ifdef __LOCAL_CONTACT__
#include "localprotocoldata.h"
using namespace QDB495;

QDB495_WrapRule::QDB495_WrapRule():WrapRule()
{

}

QByteArray QDB495_WrapRule::wrap(const QByteArray &data)
{
    QDB495_SendPackage SendPackage;
    memset(&SendPackage,0,sizeof(QDB495_SendPackage));

    SendPackage.bPackType = 1;
    SendPackage.bPackType = WM_DATA_NOAFFIRM;
    SendPackage.wPackLen = data.size();
    SendPackage.wOffset = 1;
    SendPackage.dwPackAllLen = data.size();
    SendPackage.wDestAddr = 1234;
    SendPackage.wSourceAddr = 9779 ;
    SendPackage.wSerialNo = QTime::currentTime().msecsTo(QTime(0,0,0,0))
                            +SendPackage.wDestAddr
                            +SendPackage.wSourceAddr;


    QByteArray ddsdata;
    ddsdata.append((char*)&SendPackage,sizeof(QDB495_SendPackage));
    ddsdata.append(data);
    return ddsdata;
}

QByteArray QDB495_WrapRule::unwrap(const QByteArray &data)
{
    return QByteArray(data);
}

#endif
