/*!
 *  @brief     二进制数据解析
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.26
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef BINARYPARSEFACTORY_H
#define BINARYPARSEFACTORY_H

#include "dataparse.h"

class BinaryParseFactory
{
public:
    BinaryParseFactory();

    DataParse * getDataParse();

private:
    DataParse * dataParse;
};

#endif // BINARYPARSEFACTORY_H
