#include "binaryparsefactory.h"

#include "rbuffer_msgparse.h"

BinaryParseFactory::BinaryParseFactory()
{

}

DataParse * BinaryParseFactory::getDataParse()
{
    if(dataParse == nullptr)
        dataParse = new RBuffer_MsgParse();

    return dataParse;
}
