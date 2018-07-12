#include "binary716_msgparse.h"

#include "util/rsingleton.h"
#include "../msgprocess/data716process.h"

#ifdef __LOCAL_CONTACT__

Binary716_MsgParse::Binary716_MsgParse():
    DataParse()
{

}

void Binary716_MsgParse::processData(const ProtocolPackage &packData)
{
    switch(packData.bPackType){
        case T_DATA_AFFIRM:
            {
                RSingleton<Data716Process>::instance()->processTextAffirm(packData);
            }
            break;
        case T_DATA_NOAFFIRM:
            {
                RSingleton<Data716Process>::instance()->processTextNoAffirm(packData);
            }
            break;
        case T_DATA_REG:
            break;

        default:break;
    }
}

#endif
