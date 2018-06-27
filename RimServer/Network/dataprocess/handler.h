/*!
 *  @brief     接收数据处理
 *  @details   用于在网络层数据接收完整后，交由应用层处理
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.27
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef HANDLER_H
#define HANDLER_H

#include "../head.h"

namespace ServerNetwork{

class Handler
{
public:
    Handler();

    virtual void handle(const RecvUnit & recvData) = 0;
};

}

#endif // HANDLER_H
