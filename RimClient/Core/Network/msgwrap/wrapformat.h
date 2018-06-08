/*!
 *  @brief     基本通信信息包装格式
 *  @details   抽象出针对不同的数据传输格式,当需要使用不同的数据封装格式时，继承此类并实现对应的方法
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.08
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef WRAPFORMAT_H
#define WRAPFORMAT_H

#include "protocoldata.h"
using namespace ProtocolType;

class WrapFormat
{
public:
    WrapFormat();

    virtual void handleMsg(MsgPacket * packet,QByteArray & result) = 0;
};

#endif // WRAPFORMAT_H
