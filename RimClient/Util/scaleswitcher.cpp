#include "scaleswitcher.h"

#include <WinSock2.h>
#include <QDebug>

#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif

ScaleSwitcher::ScaleSwitcher()
{

}

/*!
 * @brief 将16进制转换成10进制
 * @details 若转换成功则返回转换的信息，否则返回0
 * @param[in]  number 待转换的16进制数
 * @return 返回转换结果
 */
unsigned short ScaleSwitcher::fromHexToDec(QString &number)
{
    bool ok = false;
    unsigned short result = static_cast<unsigned short>(number.toInt(&ok,16));
    if(ok)
        return result;

    return 0;
}

/*!
 * @brief 将10进制转换成16进制
 * @warning 转换后的结果不包含0x标志，即10进制9779,对应的16进制为0x2633，此处只返回2633。 \n
 * @param[in] number 待转换的10进制数
 * @return 返回转换结果
 */
QString ScaleSwitcher::fromDecToHex(unsigned short number)
{
    return QString::number(number,16);
}

unsigned short ScaleSwitcher::htons(unsigned short number)
{
    return ::htons(number);
}

unsigned short ScaleSwitcher::ntohs(unsigned short number)
{
    return ::ntohs(number);
}

unsigned long ScaleSwitcher::htonl(unsigned long number)
{
    return ::htonl(number);
}

unsigned long ScaleSwitcher::ntohl(unsigned long number)
{
    return ::ntohl(number);
}
