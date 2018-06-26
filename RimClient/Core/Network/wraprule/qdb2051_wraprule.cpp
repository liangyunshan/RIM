#include "qdb2051_wraprule.h"
#include <QTime>

#ifdef __LOCAL_CONTACT__
#include "../../protocol/localprotocoldata.h"
using namespace QDB2051;

QDB2051_WrapRule::QDB2051_WrapRule()
{

}

QByteArray QDB2051_WrapRule::wrap(const ProtocolPackage &package)
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
                            + package.cFileData.size();

    QByteArray ddsdata;
    ddsdata.append((char*)&qdb21_2051,qdb21_2051.ulPackageLen-package.cFileData.size());
    ddsdata.append(package.cFileData);
    return ddsdata;
}

ProtocolPackage QDB2051_WrapRule::unwrap(const QByteArray &data)
{
    QDB2051_Head qdb21_2051;
    memset(&qdb21_2051,0,sizeof(QDB2051_Head));
    memcpy(&qdb21_2051,data.data(),sizeof(QDB2051_Head));

    int realsize = 0;
    realsize = qdb21_2051.ulPackageLen
            - sizeof(QDB2051_Head)
            - (int)qdb21_2051.cFilenameLen;

    QByteArray tempdata = data.right(realsize);

    ProtocolPackage package;
    package.cFileData = tempdata;
    return package;
}

#endif
