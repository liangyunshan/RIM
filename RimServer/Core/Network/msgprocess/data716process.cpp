#include "data716process.h"

#include <QDebug>

#include "../msgwrap/localmsgwrap.h"
#include "rsingleton.h"
#include "Network/connection/tcpclient.h"
#include "Network/connection/seriesconnection.h"
#include "../../protocol/datastruct.h"
#include "../../sql/sqlprocess.h"
#include "global.h"
using namespace ParameterSettings;
using namespace ServerNetwork;

#ifdef __LOCAL_CONTACT__

extern OuterNetConfig QueryNodeDescInfo(QString nodeId,bool & result);
extern NodeServer QueryServerDescInfo(QString nodeId,bool & result);

Data716Process::Data716Process()
{

}

/*!
 * @brief 处理不同的文字信息发送
 * @note  【!!服务器在接收数据后，只根据目的节点号来判断!!】 \n
 *
 *        1.若目的节点号为服务器所在的指挥中心:
 *           1.1.若源节点号不是服务器所在节点号，查找所有与服务器节点一致的客户端，并广播消息; \n
 *           1.2.若源节点号是服务器所在的节点号，通过节点号找到所有的客户端，并通过IP地址和端口号过滤掉发送源的节点号，对剩下的节点进行广播; \n
 *        2.若目的节点为非服务器所在的指挥所节点： \n
 *           2.1.查找目的节点是否在当前通信列表中，若不在则退出. \n
 *           2.2.若目的节点在当前软件通信列表中，查找目的节点所属服务器的节点： \n
 *              2.2.1.若所属服务器节点和本服务器节点一致，若在线则直接推送；若不在线，则离线缓存。 \n
 *              2.2.2.若不是同一服务器节点： \n
 *                  2.2.2.1.若对方服务器已经建立连接，则直接推送；
 *                  2.2.2.2.若对方服务器未建立连接，则
 *       3.以上的传输方式不局限于TCP，还可能为其它方式。 \n
 * @param[in] db 数据库连接
 * @param[in] sockId 网络连接
 * @param[in] data 数据信息描述
 */
void Data716Process::processText(Database *db, int sockId, ProtocolPackage &data)
{
    if(data.wDestAddr == RGlobal::G_ParaSettings->baseInfo.nodeId.toUShort()){
        ClientList clist = TcpClientManager::instance()->getClients(QString::number(data.wDestAddr));
        TcpClient * sourceClient = TcpClientManager::instance()->getClient(sockId);

        if(data.wSourceAddr == RGlobal::G_ParaSettings->baseInfo.nodeId.toUShort()){
            std::for_each(clist.begin(),clist.end(),[&](TcpClient * destClient){
                if(sourceClient && destClient->ip() != sourceClient->ip() && destClient->port() != sourceClient->port()){
                    RSingleton<LocalMsgWrap>::instance()->hanldeMsgProtcol(destClient->socket(),data);
                }
            });
        }
        else{
            if(clist.size() > 0){
                std::for_each(clist.begin(),clist.end(),[&](TcpClient * destClient){
                    RSingleton<LocalMsgWrap>::instance()->hanldeMsgProtcol(destClient->socket(),data);
                });
            }else{
                RSingleton<SQLProcess>::instance()->saveChat716Cache(db,data);
            }
        }
    }else{
        bool findNode = false;
        QueryNodeDescInfo(QString::number(data.wDestAddr),findNode);

        if(findNode){
            bool findServer = false;
            NodeServer serverInfo = QueryServerDescInfo(QString::number(data.wDestAddr),findServer);
            if(findServer){
                if(serverInfo.nodeId == RGlobal::G_ParaSettings->baseInfo.nodeId){
                    TcpClient * destClient = TcpClientManager::instance()->getClient(QString::number(data.wDestAddr));
                    if(destClient && ((OnlineStatus)destClient->getOnLineState() == STATUS_ONLINE)){
                        RSingleton<LocalMsgWrap>::instance()->hanldeMsgProtcol(destClient->socket(),data);
                    }else{
                        RSingleton<SQLProcess>::instance()->saveChat716Cache(db,data);
                    }
                }else{
                    std::shared_ptr<SeriesConnection> conn = SeriesConnectionManager::instance()->getConnection(serverInfo.nodeId);

                    if(conn.get() != nullptr){
                        RSingleton<LocalMsgWrap>::instance()->hanldeMsgProtcol(conn->socket(),data,false);
                    }else{
                        RSingleton<LocalMsgWrap>::instance()->cacheMsgProtocol(serverInfo,data);
                    }
                }
            }else{
                //TODO 20180702 目的节点不在当前通信列表，通信丢弃？？
            }
        }else{
            //TODO 20180702 目的节点不在当前通信列表，通信丢弃？？
        }
    }
}

/*!
 * @brief 处理用户注册
 * @note 接收用户消息后，若当前列表中无此客户端信息，则认为是注册信息 \n
 *       若当前用户列表中已经存在此客户端信息，则认为是注销信息.
 * @param[in] db 数据库连接
 * @param[in] sockId 网络连接
 * @param[in] data 数据信息描述
 */
void Data716Process::processUserRegist(Database *db, int sockId, ProtocolPackage &data)
{
    TcpClient * tmpClient = TcpClientManager::instance()->getClient(sockId);
    if(tmpClient){
        if((OnlineStatus)tmpClient->getOnLineState() == STATUS_OFFLINE){
            tmpClient->setAccount(QString::number(data.wSourceAddr));
            tmpClient->setOnLineState(STATUS_ONLINE);
        }else{
            tmpClient->setOnLineState(STATUS_OFFLINE);
        }
    }

//    RSingleton<LocalMsgWrap>::instance()->hanldeMsgProtol(sockId,nullptr,data);
}

#endif
