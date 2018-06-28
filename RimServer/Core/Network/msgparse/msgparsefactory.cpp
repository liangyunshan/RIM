#include "msgparsefactory.h"

#include "json_msgparse.h"

#ifdef __LOCAL_CONTACT__
#include "binary716_msgparse.h"
#endif

MsgParseFactory::MsgParseFactory():dataParse(nullptr)
{

}

DataParse *MsgParseFactory::getDataParse()
{
#ifdef __LOCAL_CONTACT__
    if(dataParse == nullptr)
        dataParse = new Binary716_MsgParse();
#else
    if(dataParse == nullptr)
        dataParse = new Json_MsgParse();
#endif

    return dataParse;
}
