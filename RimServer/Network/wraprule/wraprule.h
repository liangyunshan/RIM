/*!
 *  @brief     封装规则
 *  @details   用于多种协议的封装，每种协议内部控制封装和解封装的过程 \n
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
#include "../head.h"

namespace ServerNetwork{

class NETWORKSHARED_EXPORT WrapRule
{
public:
    WrapRule();

    virtual void wrap(ProtocolPackage & data) = 0;
    virtual bool unwrap(const QByteArray & data,ProtocolPackage & result) = 0;
};

} // namespace ClientNetwork

#endif // WRAPRULE_H
