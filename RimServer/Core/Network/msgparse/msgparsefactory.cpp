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
//<<<<<<< HEAD
//        dataParse = new Json_MsgParse();
//=======
        dataParse = new Binary716_MsgParse();
//>>>>>>> d3ffafb480aea1647881162c1a402b43ef93005c
#else
    if(dataParse == nullptr)
        dataParse = new Json_MsgParse();
#endif

    return dataParse;
}
