#include "binary716_msgparse.h"

#include "rsingleton.h"
#include "../msgprocess/data716process.h"
#include "Network/wraprule/tcp_wraprule.h"

#include <QDebug>

#ifdef __LOCAL_CONTACT__

Binary716_MsgParse::Binary716_MsgParse():
    DataParse()
{

}

/*!
 * @brief 处理网络层接收的数据信息
 * @note 此方法作为处理网络层数据的入口，具备对文本信息和文件信息分发处理的能力。 \n
 *       1.处理文本信息：分析发送的目的节点，根据离/在线情况、是否为同一节点进行不同处理 \n
 *       2.处理文件信息：将数据写入本地磁盘，并记录已经接收的状态，并周期性回复一定的状态信息 \n
 * @param[in] db 数据库连接
 * @param[in] data 待处理的是数据单元
 */
void Binary716_MsgParse::processData(Database *db, const RecvUnit &unit)
{
    ProtocolPackage packData;

    if(unit.extendData.method == C_TCP)
    {
        packData.bPackType = unit.extendData.type495;
        packData.bPeserve = unit.extendData.bPeserve;
        packData.wOffset = unit.extendData.wOffset;
        packData.dwPackAllLen = unit.extendData.dwPackAllLen;
        packData.usSerialNo = unit.extendData.usSerialNo;
        packData.wSourceAddr = unit.extendData.wSourceAddr;
        packData.wDestAddr = unit.extendData.wDestAddr;

        //单独495协议
        if(checkHead495Only(unit.extendData.type495))
        {
            RSingleton<Data716Process>::instance()->processUserRegist(db,unit.extendData.sockId,packData);
        }
        else
        {
            //文本信息
            if(unit.extendData.type == SOCKET_TEXT){
                if(RSingleton<ServerNetwork::TCP_WrapRule>::instance()->unwrap(unit.data,packData)){
                    if(packData.cFileType == QDB2051::F_NO_SUFFIX){
                        switch(unit.extendData.type495){
                            case T_DATA_AFFIRM:
                            case T_DATA_NOAFFIRM:
                                RSingleton<Data716Process>::instance()->processText(db,unit.extendData.sockId,packData);
                                break;
                            default:break;
                        }
                    }
                }
            }
            //文件信息
            else if(unit.extendData.type == SOCKET_FILE){
                //可能是第一包数据，也可能就一包数据
                if(unit.extendData.wOffset == 0){
                    if(RSingleton<ServerNetwork::TCP_WrapRule>::instance()->unwrap(unit.data,packData)){

                    }
                }else{
                    packData.data = unit.data;
                }

                RSingleton<Data716Process>::instance()->processFileData(db,unit.extendData.sockId,packData);
            }
        }
    }
}

/*!
 * @brief 根据数据包类型判断数据头是否需要进一步解析
 * @param[in] type 数据包类型
 * @return true  此数据只包含495协议 \n
 *         false 此数据需要解析21、2051或其他数据头
 */
bool Binary716_MsgParse::checkHead495Only(const PacketType_495 type)
{
    return (type == T_DATA_REG);
}

#endif
