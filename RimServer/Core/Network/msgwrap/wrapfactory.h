/*!
 *  @brief     文本信息封装工厂类
 *  @details   为了降低耦合性，提高替换的灵活性，采用工厂模式为不同的文本信息封装方式提供统一的调用接口。 \n
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.25
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

    MsgWrap * getWrap();

private:
    MsgWrap * msgWrap;

};

#endif // TEXTWRAPFACTORY_H
