#include "qdb21_wraprule.h"
#include <QTime>

#ifdef __LOCAL_CONTACT__
#include "protocol/localprotocoldata.h"
using namespace QDB21;

QDB21_WrapRule::QDB21_WrapRule():WrapRule()
{

}

QByteArray QDB21_WrapRule::wrap(const QByteArray &data)
{
    QDB21_Head qdb21_Head;
    memset(&qdb21_Head,0,sizeof(QDB21_Head));
    qdb21_Head.cTypeNum =1;
    qdb21_Head.ulPackageLen = sizeof(QDB21_Head) + data.size();
    qdb21_Head.usOrderNo = 2051;

    QByteArray wrap;
    wrap.append((char*)&qdb21_Head,sizeof(QDB21_Head));
    wrap.append(data);
    return wrap;
}

QByteArray QDB21_WrapRule::unwrap(const QByteArray &data)
{
    return QByteArray(data);
}

#endif
