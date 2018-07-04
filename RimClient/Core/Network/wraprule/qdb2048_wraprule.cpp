#include "qdb2048_wraprule.h"


#ifdef __LOCAL_CONTACT__
#include "../../protocol/localprotocoldata.h"
using namespace QDB2048;

QDB2048_WrapRule::QDB2048_WrapRule()
{

}

void QDB2048_WrapRule::wrap(ProtocolPackage &data)
{
    QDB2048_Head header;
    memset(&header,0,QDB2048_Head_Length);
    header.ulDestDeviceNo = data.wDestAddr;
    header.usDestSiteNo = data.wDestAddr;
    header.usSerialNo = data.usSerialNo;
    header.usOrderNo = 2048;
    header.ulPackageLen = QDB2048_Head_Length + data.data.size();

    data.data.prepend((char*)&header,QDB2048_Head_Length);
}

bool QDB2048_WrapRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    if(data.size() < QDB2048_Head_Length)
        return false;

    QDB2048_Head header;
    memset(&header,0,QDB2048_Head_Length);
    memcpy(&header,data.data(),QDB2048_Head_Length);

    result.data = data.right(header.ulPackageLen-QDB2048_Head_Length);
    return true;
}

#endif
