/*!
 *  @brief     二进制格式封装工厂
 *  @details   为了降低耦合性，提高替换的灵活性，采用工厂模式为不同的文本信息封装方式提供统一的调用接口。 \n
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.25
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef BINARYWRAPFACTORY_H
#define BINARYWRAPFACTORY_H

#include "msgwrap.h"

class BinaryWrapFactory
{
public:
    explicit BinaryWrapFactory();

    MsgWrap * getTextWrap();

private:
    MsgWrap * msgWrap;
};

#endif // BINARYWRAPFACTORY_H
