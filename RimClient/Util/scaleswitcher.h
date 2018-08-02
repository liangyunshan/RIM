/*!
 *  @brief     进制转换工具
 *  @details   满足将十进制转换为十六进制
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.01
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef SCALESWITCHER_H
#define SCALESWITCHER_H

#include "util_global.h"

#include <QString>

class UTILSHARED_EXPORT ScaleSwitcher
{
public:
    ScaleSwitcher();

    static unsigned short fromHexToDec(QString & number);
    static QString fromDecToHex(unsigned short number);

    static unsigned short htons(unsigned short number);
    static unsigned short ntohs(unsigned short number);
    static unsigned long htonl(unsigned long number);
    static unsigned long ntohl(unsigned long number);
};

#endif // SCALESWITCHER_H
