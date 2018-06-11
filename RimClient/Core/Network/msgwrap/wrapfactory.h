/*!
 *  @brief     数据包装工厂类
 *  @details   可快速切换不同的数据封装格式
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.08
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef WRAPFACTORY_H
#define WRAPFACTORY_H

#include "msgwrap.h"

class WrapFactory
{
public:
    explicit WrapFactory();

    MsgWrap * getMsgWrap();

private:
    MsgWrap * msgWrap;
};

#endif // WRAPFACTORY_H
