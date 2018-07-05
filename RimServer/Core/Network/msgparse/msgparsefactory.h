/*!
 *  @brief     数据格式解析工厂
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.25
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGPARSEFACTORY_H
#define MSGPARSEFACTORY_H

#include "dataparse.h"

class MsgParseFactory
{
public:
    MsgParseFactory();

    DataParse * getDataParse();

private:
    DataParse * dataParse;

};

#endif // MSGPARSEFACTORY_H
