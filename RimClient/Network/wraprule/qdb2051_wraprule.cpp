#include "qdb2051_wraprule.h"

#include <QTime>

#ifdef __LOCAL_CONTACT__
#include "network_global.h"
using namespace QDB2051;

namespace ClientNetwork{

QDB2051_WrapRule::QDB2051_WrapRule()
{

}

/*!
 *  @brief  采用2051协议进行数据包裹，默认2051的协议不包含文件名。
 */
void QDB2051_WrapRule::wrap(ProtocolPackage & package)
{
    QDB2051_Head qdb21_2051;
    memset(&qdb21_2051,0,sizeof(QDB2051_Head));
    qdb21_2051.cFileType = package.cFileType;
    qdb21_2051.cFilenameLen = package.cFilename.length();
    qdb21_2051.ulDestDeviceNo = package.wDestAddr;
    qdb21_2051.usDestSiteNo = package.wDestAddr;
    qdb21_2051.ulPackageLen = sizeof(qdb21_2051.ulPackageLen)
                            + sizeof(qdb21_2051.usDestSiteNo)
                            + sizeof(qdb21_2051.ulDestDeviceNo)
                            + sizeof(qdb21_2051.cFileType)
                            + sizeof(qdb21_2051.cFilenameLen)
                            + qdb21_2051.cFilenameLen
                            + package.data.size();

    if(qdb21_2051.cFilenameLen > 0){
        package.data.prepend(package.cFilename);
    }

    package.data.prepend((char*)&qdb21_2051,qdb21_2051.ulPackageLen-package.data.size());
}

bool QDB2051_WrapRule::unwrap(const QByteArray & data,ProtocolPackage & result)
{
    if(data.size() < sizeof(QDB2051_Head))
        return false;

    QDB2051_Head qdb21_2051;
    memset(&qdb21_2051,0,sizeof(QDB2051_Head));
    memcpy(&qdb21_2051,data.data(),sizeof(QDB2051_Head));

    int realsize = 0;
    realsize = data.size() - sizeof(QDB2051_Head) - (int)qdb21_2051.cFilenameLen;

    result.cFileType = qdb21_2051.cFileType;
    if(qdb21_2051.cFilenameLen > 0){
        result.cFilename.append(data.data()+sizeof(QDB2051_Head),qdb21_2051.cFilenameLen);
    }

    result.data = data.right(realsize);
    return true;
}

} //namespace ClientNetwork

#endif
