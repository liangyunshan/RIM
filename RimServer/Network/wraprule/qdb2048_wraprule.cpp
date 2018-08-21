#include "qdb2048_wraprule.h"


#ifdef __LOCAL_CONTACT__
#include "head.h"
using namespace QDB2048;

namespace ServerNetwork{

QDB2048_WrapRule::QDB2048_WrapRule()
{

}

void QDB2048_WrapRule::wrap(ProtocolPackage &data)
{
    QDB2048_Head header;
    memset(&header,0,QDB2048_Head_Length);
    header.destDeviceNo = data.pack495.destAddr;
    header.destSiteNo = data.pack495.destAddr;
    header.serialNo = data.pack495.serialNo;
    header.orderNo = 2048;
    header.packageLen = QDB2048_Head_Length + data.data.size();

    data.data.prepend((char*)&header,QDB2048_Head_Length);
}

bool QDB2048_WrapRule::unwrap(const QByteArray &data, ProtocolPackage &result)
{
    if(data.size() < QDB2048_Head_Length)
        return false;

    QDB2048_Head header;
    memset(&header,0,QDB2048_Head_Length);
    memcpy(&header,data.data(),QDB2048_Head_Length);

    result.data = data.right(header.packageLen - QDB2048_Head_Length);
    return true;
}

}

#endif
