#include "binary716_msgparse.h"

#include "rsingleton.h"
#include "../msgprocess/data716process.h"
#include "../wraprule/tcp_wraprule.h"

#ifdef __LOCAL_CONTACT__

Binary716_MsgParse::Binary716_MsgParse():
    DataParse()
{

}

/*!
 * @brief 处理网络层接收的数据信息
 * @param[in] db 数据库连接
 * @param[in] data 待处理的是数据单元
 */
void Binary716_MsgParse::processData(Database *db, const RecvUnit &unit)
{
    ProtocolPackage packData;
    if(RSingleton<TCP_WrapRule>::instance()->unwrap(unit.data,packData)){

        switch(unit.extendData.type495){
            case T_DATA_AFFIRM:
            case T_DATA_NOAFFIRM:
                RSingleton<Data716Process>::instance()->processText(db,unit.extendData.sockId,packData);
                break;
            case T_DATA_REG:
                RSingleton<Data716Process>::instance()->processUserRegist(db,unit.extendData.sockId,packData);
                break;

            default:break;
        }
    }
}

#endif
