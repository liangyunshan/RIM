/*!
 *  @brief     封装规则
 *  @details   用于多种协议的封装，每种协议内部控制封装和解封装的过程 \n
 *
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.07
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef WRAPRULE_H
#define WRAPRULE_H

#include <QByteArray>

#include "../network_global.h"

namespace ClientNetwork{

class NETWORKSHARED_EXPORT WrapRule
{
public:
    WrapRule();

    virtual QByteArray wrap(const ProtocolPackage & data) = 0;
    virtual QByteArray unwrap(const QByteArray & data) = 0;
};

} // namespace ClientNetwork

#endif // WRAPRULE_H
