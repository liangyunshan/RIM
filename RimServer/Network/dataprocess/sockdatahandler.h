﻿/*!
 *  @brief
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.X27
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SOCKDATAHANDLER_H
#define SOCKDATAHANDLER_H

#include "handler.h"

namespace ServerNetwork{

class SockDataHandler : public Handler
{
public:
    SockDataHandler();

    void handle(const RecvUnit &recvData);

};

}

#endif // SOCKDATAHANDLER_H
