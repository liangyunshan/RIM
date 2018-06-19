#include "qdb2051_wraprule.h"
#include <QTime>
#include <QDebug>

#ifdef __LOCAL_CONTACT__
#include "localprotocoldata.h"
using namespace QDB2051;

QDB2051_WrapRule::QDB2051_WrapRule()
{

}

QByteArray QDB2051_WrapRule::wrap(const QByteArray &data)
{
    QDB2051_Head qdb21_2051;
    memset(&qdb21_2051,0,sizeof(QDB2051_Head));
    qdb21_2051.cFileType = 0;
    qdb21_2051.cFilenameLen = 0;
    qdb21_2051.ulDestDeviceNo = QTime::currentTime().msecsTo(QTime(0,0,0,0));
    qdb21_2051.ulPackageLen = sizeof(qdb21_2051.ulPackageLen)
                            + sizeof(qdb21_2051.usDestSiteNo)
                            + sizeof(qdb21_2051.ulDestDeviceNo)
                            + sizeof(qdb21_2051.cFileType)
                            + sizeof(qdb21_2051.cFilenameLen)
                            + qdb21_2051.cFilenameLen
                            + data.size();

    QByteArray ddsdata;
    ddsdata.append((char*)&qdb21_2051,qdb21_2051.ulPackageLen-data.size());
    ddsdata.append(data);
    return ddsdata;
}

QByteArray QDB2051_WrapRule::unwrap(const QByteArray &data)
{
    return QByteArray(data);
}

#endif
