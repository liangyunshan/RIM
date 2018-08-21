#include "qdb2051_wraprule.h"
#include <QTime>

#ifdef __LOCAL_CONTACT__
#include "head.h"
using namespace QDB2051;

namespace ServerNetwork{

QDB2051_WrapRule::QDB2051_WrapRule()
{

}

void QDB2051_WrapRule::wrap(ProtocolPackage & package)
{
    QDB2051_Head qdb21_2051;
    memset(&qdb21_2051,0,sizeof(QDB2051_Head));
    qdb21_2051.fileType = package.fileType;
    qdb21_2051.filenameLen = package.filename.length();
    qdb21_2051.destDeviceNo = package.pack495.destAddr;
    qdb21_2051.destSiteNo = package.pack495.destAddr;
    qdb21_2051.packageLen = sizeof(qdb21_2051.packageLen)
                            + sizeof(qdb21_2051.destSiteNo)
                            + sizeof(qdb21_2051.destDeviceNo)
                            + sizeof(qdb21_2051.fileType)
                            + sizeof(qdb21_2051.filenameLen)
                            + qdb21_2051.filenameLen
                            + package.data.size();

    package.data.prepend(package.filename);
    package.data.prepend((char*)&qdb21_2051,qdb21_2051.packageLen-package.data.size());
}

bool QDB2051_WrapRule::unwrap(const QByteArray & data,ProtocolPackage & result)
{
    if(data.size() < sizeof(QDB2051_Head))
        return false;

    QDB2051_Head qdb21_2051;
    memset(&qdb21_2051,0,sizeof(QDB2051_Head));
    memcpy(&qdb21_2051,data.data(),sizeof(QDB2051_Head));

    int realsize = 0;
    realsize = data.size() - sizeof(QDB2051_Head) - (int)qdb21_2051.filenameLen;

    result.fileType = qdb21_2051.fileType;
    if(qdb21_2051.filenameLen > 0){
        result.filename.append(data.data()+sizeof(QDB2051_Head),qdb21_2051.filenameLen);
    }
    result.data = data.right(realsize);
    return true;
}

} //namespace ServerNetwork

#endif
