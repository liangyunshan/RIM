#include "qdb21_wraprule.h"
#include <QTime>

#ifdef __LOCAL_CONTACT__
#include "protocol/localprotocoldata.h"
using namespace QDB21;

QDB21_WrapRule::QDB21_WrapRule():WrapRule()
{

}

void QDB21_WrapRule::wrap(ProtocolPackage & data)
{
    QDB21_Head qdb21_Head;
    memset(&qdb21_Head,0,sizeof(QDB21_Head));
    qdb21_Head.usDestAddr = data.wDestAddr;
    qdb21_Head.usSourceAddr = data.wSourceAddr;
    qdb21_Head.cTypeNum =1;
    qdb21_Head.ulPackageLen = sizeof(QDB21_Head) + data.data.size();
    qdb21_Head.usOrderNo = 2051;

    data.data.prepend((char*)&qdb21_Head,sizeof(QDB21_Head));
}

bool QDB21_WrapRule::unwrap(const QByteArray & data,ProtocolPackage & result)
{
    if(data.size() < sizeof(QDB21_Head))
        return false;

    QDB21_Head qdb21_Head;
    memset(&qdb21_Head,0,sizeof(QDB21_Head));
    memcpy(&qdb21_Head,data.data(),sizeof(QDB21_Head));

    result.data = data.right(data.size() - QDB21_Head_Length);
    result.wDestAddr = qdb21_Head.usDestAddr;
    result.wSourceAddr = qdb21_Head.usSourceAddr;

    return true;
}

#endif
