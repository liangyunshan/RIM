#include "binary716_msgparse.h"

#ifdef __LOCAL_CONTACT__

#include <QDebug>

#include "rsingleton.h"
#include "../../protocol/datastruct.h"
#include "../msgprocess/data716process.h"
#include "Network/wraprule/tcp_wraprule.h"
#include "Network/wraprule/qdb21_wraprule.h"
#include "Network/connection/tcpclient.h"
#include "../../protocol/protocoldata.h"
#include "global.h"

using namespace QDB21;
using namespace ParameterSettings;
using namespace ServerNetwork;

extern NodeClient * QueryNodeDescInfo(unsigned short nodeId,bool & result);
extern NodeServer * QueryServerDescInfoByClient(unsigned short nodeId,bool & result);

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

    //【1】对未发送注册报的节点学习记录
    TcpClient * client = TcpClientManager::instance()->getClient(unit.extendData.sockId);
    if(client){
        if(client->getAccount().size() == 0){
            client->setAccount(QString::number(unit.extendData.pack495.sourceAddr));
            client->setOnLineState(ProtocolType::STATUS_ONLINE);
        }
    }else{
        return;
    }

    if(unit.extendData.method == C_TCP)
    {
        packData.method = unit.extendData.method;
        packData.pack495 = unit.extendData.pack495;

        //文本信息
        if(unit.extendData.type == SOCKET_TEXT){

            //走正常21、2051协议解析，若解析失败则直接走转发
            bool normalParsedMethod = false;

            if(checkHead495Only(unit)){
                normalParsedMethod = true;
                packData.data = unit.data,packData;
                packData.orderNo = 0;
            }else{
                normalParsedMethod = RSingleton<ServerNetwork::TCP_WrapRule>::instance()->unwrap(unit.data,packData);
            }

            if(normalParsedMethod && packData.fileType == QDB2051::F_NO_SUFFIX)
            {
                switch(static_cast<PacketType_495>(packData.pack495.packType)){
                    case T_DATA_AFFIRM:
                    case T_DATA_NOAFFIRM:
                        RSingleton<Data716Process>::instance()->processText(db,unit.extendData.sockId,packData);
                        break;
                    case T_DATA_REG:
                    case T_CONNECT_TEST:
                        {
                            RSingleton<Data716Process>::instance()->processTranspondData(db,unit.extendData.sockId,packData);
                        }
                        break;
                    default:
                        {
                            RSingleton<Data716Process>::instance()->processTranspondData(db,unit.extendData.sockId,packData);
                        }
                        break;
                }
            }
            //495转发通道
            else
            {
                packData.data = unit.data;
                RSingleton<Data716Process>::instance()->processForwardData(db,unit.extendData.sockId,packData);
            }
        }
        //文件信息
        else if(unit.extendData.type == SOCKET_FILE)
        {
            //可能是第一包数据，也可能就一包数据
            if(unit.extendData.pack495.offset == 0){
                if(RSingleton<ServerNetwork::TCP_WrapRule>::instance()->unwrap(unit.data,packData)){

                }
            }else{
                packData.data = unit.data;
            }

            RSingleton<Data716Process>::instance()->processFileData(db,unit.extendData.sockId,packData);
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

bool Binary716_MsgParse::checkHead495Only(RecvUnit unit)
{
    if(checkHeadHave21(unit))
    {
        return false;
    }
    return true;
}

bool Binary716_MsgParse::checkHeadHave21(RecvUnit unit)
{
    if(unit.data.size()<QDB21_Head_Length)
    {
        return false;
    }
    QDB21::QDB21_Head header;
    memcpy(&header,unit.data.data(),QDB21_Head_Length);
    if(header.destAddr == unit.extendData.pack495.destAddr)
    {
        return true;
    }
    return false;
}

#endif
