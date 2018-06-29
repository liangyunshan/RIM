#include "binaryparsefactory.h"

#include "rbuffer_msgparse.h"

BinaryParseFactory::BinaryParseFactory():
    dataParse(nullptr)
{

}

DataParse * BinaryParseFactory::getDataParse()
{
    if(dataParse == nullptr)
        dataParse = new RBuffer_MsgParse();

    return dataParse;
}
