#include "binary716_msgparse.h"

#include "rsingleton.h"
#include "../wraprule/tcp_wraprule.h"
#include "../msgprocess/data716process.h"
#include <QDebug>

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
void Binary716_MsgParse::processData(const RecvUnit &unit)
{
    ProtocolPackage packData;
    if(RSingleton<TCP_WrapRule>::instance()->unwrap(unit.data,packData)){
        packData.bPackType = unit.extendData.type495;
        packData.bPeserve = unit.extendData.bPeserve;
        switch(unit.extendData.type495){
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
}

#endif
