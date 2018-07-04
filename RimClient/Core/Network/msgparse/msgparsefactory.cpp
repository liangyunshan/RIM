#include "msgparsefactory.h"

#ifdef __LOCAL_CONTACT__
    #include "binary716_msgparse.h"
#else
    #include "json_msgparse.h"
#endif

MsgParseFactory::MsgParseFactory():dataParse(nullptr)
{

}

DataParse *MsgParseFactory::getDataParse()
{
    if(dataParse == nullptr)
    {
#ifdef __LOCAL_CONTACT__
        dataParse = new Binary716_MsgParse();
#else
        dataParse = new Json_MsgParse();
#endif
    }

    return dataParse;
}
