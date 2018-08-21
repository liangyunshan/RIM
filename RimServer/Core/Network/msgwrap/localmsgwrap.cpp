#include "localmsgwrap.h"

#ifdef __LOCAL_CONTACT__

#include "Network/head.h"
#include "Network/netglobal.h"
#include "binary716_wrapfromat.h"
#include "rsingleton.h"
#include "global.h"

#include <QDebug>

#include "../../protocol/datastruct.h"
using namespace ParameterSettings;

std::mutex QueryNodeMutex;

/*!
 * @brief 根据节点查找对应节点的相信通信配置信息.
 * @note  [1]装备之间的通信可通过外发信息配置来约束通信方式; \n
 *        [2]指挥所内部的通信目前暂定使用TCP方式信息传输。
 * @param[in] nodeId 目的节点号
 * @param[in] result 查找结果，true表示查找成功，false表示查找失败
 * @return 所查找的节点号信息
 */
NodeClient QueryNodeDescInfo(unsigned short nodeId,bool & result){
    std::lock_guard<std::mutex> lg(QueryNodeMutex);
    result = false;

    auto findIndex = std::find_if(RGlobal::G_RouteSettings->clients.begin(),RGlobal::G_RouteSettings->clients.end(),
                 [&nodeId](const NodeClient & client){

        if(client.nodeId == nodeId)
            return true;
        return false;
    });

    if(findIndex != RGlobal::G_RouteSettings->clients.end()){
        result = true;
        return *findIndex;
    }

    if(nodeId == RGlobal::G_RouteSettings->baseInfo.nodeId){
        result = true;
        NodeClient client;
        client.nodeId = nodeId;
        client.communicationMethod = C_TongKong;
        client.messageFormat = M_495;
        return client;
    }

    return NodeClient();
}

/*!
 * @brief 根据客户端节点查询节点所属服务器节点
 * @param[in] nodeId 待查询的节点
 * @param[in] result 配置文件中是否存在当前服务器的父节点
 * @return 查找的服务器节点
 */
NodeServer QueryServerDescInfoByClient(unsigned short nodeId,bool & result){
    result = false;

//    qDebug()<<"========start=========";
//    std::for_each(RGlobal::G_RouteSettings->clients.begin(),RGlobal::G_RouteSettings->clients.end(),[](const NodeClient & client){
//        qDebug()<<"Client:"<<client.nodeId<<"__"<<client.serverNodeId;
//    });
//    qDebug()<<"========end=========";

    auto clientIndex = std::find_if(RGlobal::G_RouteSettings->clients.begin(),RGlobal::G_RouteSettings->clients.end(),[&nodeId](const NodeClient & client){
        return (client.nodeId == nodeId);
    });

    if(clientIndex != RGlobal::G_RouteSettings->clients.end()){
        auto serverIndex = std::find_if(RGlobal::G_RouteSettings->servers.begin(),RGlobal::G_RouteSettings->servers.end(),[&](const NodeServer & server){
              return server.nodeId == (*clientIndex).serverNodeId;
        });

        if(serverIndex != RGlobal::G_RouteSettings->servers.end()){
            result = true;
            return (*serverIndex);
        }
    }

    return NodeServer();
}

/*!
 * @brief 根据服务器节点查询服务器详细信息
 * @param[in] nodeId 待查询的服务器节点
 * @param[in] result 配置文件中是否存在当前服务器节点
 * @return 查找的服务器节点
 */
NodeServer QueryServerDescInfoByServerNodeId(unsigned short nodeId,bool & result){
    result = false;

    auto serverIndex = std::find_if(RGlobal::G_RouteSettings->servers.begin(),RGlobal::G_RouteSettings->servers.end(),[&nodeId](const NodeServer & server){
        return (server.nodeId == nodeId);
    });

    if(serverIndex != RGlobal::G_RouteSettings->servers.end()){
        result = true;
        return (*serverIndex);
    }

    return NodeServer();
}

LocalMsgWrap::LocalMsgWrap():MsgWrap()
{

}

/*!
 * @brief 处理待发送数据
 * @details 将待发送的数据信息，按照目的节点通信方式和链路进行数据封装，最终将封装后的数据压入待发送的数据缓冲区
 * @param[in] sockId 通信socket连接
 * @param[in] package 待数据封装的原始数据
 * @param[in] inServer 目的数据是否为同一服务器节点，默认为true。
 */
void LocalMsgWrap::hanldeMsgProtcol(int sockId,ProtocolPackage & package,bool inServer)
{
    SendUnit sUnit;
    sUnit.sockId = sockId;
    sUnit.method = C_NONE;
    sUnit.localServer = inServer;
    sUnit.dataUnit = package;

    if(inServer){
        bool flag = false;
        NodeClient nodeClient = QueryNodeDescInfo(package.pack495.destAddr,flag);
        if(flag){
            if(nodeClient.communicationMethod == C_NetWork && nodeClient.messageFormat == M_205){
                sUnit.method = C_UDP;
            }else if(nodeClient.communicationMethod == C_TongKong && nodeClient.messageFormat == M_495){
                sUnit.method = C_TCP;
            }
        }else{
            if(package.method != C_NONE){
                sUnit.method = package.method;
            }else{
                //TODO 错误记录
            }
        }
    }else{
        bool findServer = false;
        NodeServer serverInfo = QueryServerDescInfoByClient(package.pack495.destAddr,findServer);

        if(!findServer){
            serverInfo = QueryServerDescInfoByServerNodeId(package.pack495.destAddr,findServer);
        }

        if(findServer){
            if(serverInfo.communicationMethod == C_NetWork && serverInfo.messageFormat == M_205){
                sUnit.method = C_UDP;
            }else if(serverInfo.communicationMethod == C_TongKong && serverInfo.messageFormat == M_495){
                sUnit.method = C_TCP;
            }
        }else{
            if(package.method != C_NONE){
                sUnit.method = package.method;
            }else{
                //TODO 错误记录
            }
        }
    }

    if(sUnit.method != C_NONE){
        std::unique_lock<std::mutex> ul(G_SendMutex);
        G_SendButts.push(sUnit);
        G_SendCondition.notify_one();
    }
}

void LocalMsgWrap::handleMsgReply(int sockId, MsgType type, MsgCommand command, int replyCode, int subMsgCommand)
{

}

#endif
