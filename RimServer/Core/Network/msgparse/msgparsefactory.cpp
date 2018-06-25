#include "msgparsefactory.h"

#include "json_msgparse.h"

MsgParseFactory::MsgParseFactory():dataParse(nullptr)
{

}

DataParse *MsgParseFactory::getDataParse()
{
    if(dataParse == nullptr)
        dataParse = new Json_MsgParse();

    return dataParse;
}
