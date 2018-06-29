#include "data716process.h"

#include "../msgwrap/localmsgwrap.h"
#include "rsingleton.h"
#include "Network/tcpclient.h"
#include "global.h"
#include "../../protocol/datastruct.h"
using namespace ParameterSettings;

using namespace ServerNetwork;

#ifdef __LOCAL_CONTACT__

extern OuterNetConfig queryNodeDescInfo(QString nodeId,bool & result);

Data716Process::Data716Process()
{

}

/*!
 * @brief 处理不同的文字信息发送
 * @note  【!!服务器在接收数据后，只根据目的节点号来判断!!】
 *        1.若目的节点号为服务器所在的指挥中心，那么需要将所有与服务器节点一致的客户端找到，并广播消息; \n
 *        2.若目的节点为非服务器所在的指挥所： \n
 *           a.对方可通过服务器的配置表直接找到 \n
 *              I.若对方在线，则直接投递消息； \n
 *              II.若对方未在线，则将消息缓存； \n
 *           b.对方不在服务器的配置表中，需要服务器解析【串联配置表】，获取对方客户端所在的服务器节点： \n
 *              I.若对方服务器未在Clients客户端列表中，则主动发送连接请求 \n
 *                  A.若连接成功，则将消息转发至对端服务器； \n
 *                  B.若连接不成功，则将消息存储，待下次再连接或者对方连接至本服务器后，再一并推送。 \n
 *              II.若对方服务器已经与当前服务器建立连接 \n
 *                  A.直接将消息转发至对端服务器。\n
 *       3.以上的传输方式不局限于TCP，还可能为其它方式。 \n
 * @param[in] db 数据库连接
 * @param[in] sockId 网络连接
 * @param[in] data 数据信息描述
 */
void Data716Process::processText(Database *db, int sockId, ProtocolPackage &data)
{
    if(data.wDestAddr == RGlobal::G_ParaSettings->baseInfo.nodeId.toUShort()){
        ClientList clist = TcpClientManager::instance()->getClients(QString::number(data.wDestAddr));
        if(clist.size() > 0){
            std::for_each(clist.begin(),clist.end(),[&](TcpClient * destClient){
                RSingleton<LocalMsgWrap>::instance()->hanldeMsgProtol(destClient->socket(),data);
            });
        }else{
            //TODO 20180628 存储至离线数据库
        }
    }else{
        bool finded = false;
        queryNodeDescInfo(QString::number(data.wDestAddr),finded);
        if(finded){
            TcpClient * destClient = TcpClientManager::instance()->getClient(QString::number(data.wDestAddr));
            if(destClient && ((OnlineStatus)destClient->getOnLineState() == STATUS_ONLINE)){
                RSingleton<LocalMsgWrap>::instance()->hanldeMsgProtol(destClient->socket(),data);
            }else{
                //TODO 20180628 存储离线数据库
            }
        }
        //根据串联配置表查找对应的节点信息
        else{

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
