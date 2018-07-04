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
 * @param[in] data 待处理的是数据单元
 */
void Binary716_MsgParse::processData(const ProtocolPackage &recvData)
{
    switch(recvData.bPackType){
        case T_DATA_AFFIRM:
            {
                RSingleton<Data716Process>::instance()->processTextAffirm(recvData);
            }
            break;
        case T_DATA_NOAFFIRM:
            {
                RSingleton<Data716Process>::instance()->processTextNoAffirm(recvData);
            }
            break;
        case T_DATA_REG:
            break;

        default:break;
    }
}

#endif
