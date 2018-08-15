#include "qdb21_wraprule.h"

#ifdef __LOCAL_CONTACT__
#include "head.h"
using namespace QDB21;

namespace ServerNetwork{

QDB21_WrapRule::QDB21_WrapRule():WrapRule()
{

}

void QDB21_WrapRule::wrap(ProtocolPackage & data)
{
    QDB21_Head qdb21_Head;
    memset(&qdb21_Head,0,sizeof(QDB21_Head));
    qdb21_Head.destAddr = data.pack495.destAddr;
    qdb21_Head.sourceAddr = data.pack495.sourceAddr;
    qdb21_Head.typeNum =1;
    qdb21_Head.packageLen = sizeof(QDB21_Head) + data.data.size();
    qdb21_Head.orderNo = data.orderNo;
    qdb21_Head.serialNo = data.pack495.serialNo;

    wrapTime(qdb21_Head.date,data.date,4);
    wrapTime(qdb21_Head.time,data.time,3);

    data.data.prepend((char*)&qdb21_Head,sizeof(QDB21_Head));
}

bool QDB21_WrapRule::unwrap(const QByteArray & data,ProtocolPackage & result)
{
    if(data.size() < QDB21_Head_Length)
        return false;

    QDB21_Head qdb21_Head;
    memset(&qdb21_Head,0,QDB21_Head_Length);
    memcpy(&qdb21_Head,data.data(),QDB21_Head_Length);

    if(qdb21_Head.sourceAddr != result.pack495.sourceAddr || qdb21_Head.destAddr != result.pack495.destAddr)
        return false;

    result.data = data.right(data.size() - QDB21_Head_Length);
    result.pack495.destAddr = qdb21_Head.destAddr;
    result.pack495.sourceAddr = qdb21_Head.sourceAddr;
    result.pack495.serialNo = qdb21_Head.serialNo;
    result.orderNo = qdb21_Head.orderNo;

    unwrapTime(result.date,qdb21_Head.date,4);
    unwrapTime(result.time,qdb21_Head.time,3);

    return true;
}

/*!
 * @brief 对时间进行转换
 * @note 利用对整型的移位操作，实现类似socket中inet_addr()的转换
 * @param[in] start 起始的地址
 * @param[in] value 值
 * @param[in] length 数据长度(字节数)
 */
void QDB21_WrapRule::wrapTime(char *output, int intput, int length)
{
    if(output == NULL)
        return;

    int loopTime = 0;
    int ptrInt = intput;
    while(loopTime < length)
    {
        output[loopTime] =(char) ptrInt;
        ptrInt = ptrInt >> 8;
        loopTime++;
    }
}

/*!
 * @brief 对时间进行进行解压
 * @note 利用对整型的移位操作，实现类似socket中inet_addr()的转换
 * @param[in] start 起始的地址
 * @param[in] value 值
 * @param[in] length 数据长度(字节数)
 */
void QDB21_WrapRule::unwrapTime(int &output, char *input, int length)
{
    if(input == NULL)
        return;

    memset((char *)&output,0,sizeof(int));
    memcpy((char *)&output,input,length);
}

} //ServerNetwork

#endif
