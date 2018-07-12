#include "localmsgwrap.h"

#ifdef __LOCAL_CONTACT__

#include <QDebug>

#include "Network/head.h"
#include "Network/netglobal.h"
#include "binary716_wrapfromat.h"
#include "rsingleton.h"
#include "global.h"

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
OuterNetConfig QueryNodeDescInfo(QString nodeId,bool & result){
    std::lock_guard<std::mutex> lg(QueryNodeMutex);
    result = false;

    auto findIndex = std::find_if(RGlobal::G_ParaSettings->outerNetConfig.begin(),RGlobal::G_ParaSettings->outerNetConfig.end(),
                 [&nodeId](const OuterNetConfig & config){
        if(config.nodeId == nodeId)
            return true;
        return false;
    });

    if(findIndex != RGlobal::G_ParaSettings->outerNetConfig.end()){
        result = true;
        return *findIndex;
    }

    if(nodeId == RGlobal::G_ParaSettings->baseInfo.nodeId){
        result = true;
        OuterNetConfig localConfig;
        localConfig.nodeId = nodeId;
        localConfig.communicationMethod = C_TongKong;
        localConfig.messageFormat = M_495;
        return localConfig;
    }

    return OuterNetConfig();
}

/*!
 * @brief 查询节点所属服务器节点
 * @param[in] nodeId 待查询的节点
 * @param[in] result 配置文件中是否存在当前服务器的父节点
 * @return 查找的服务器节点
 */
NodeServer QueryServerDescInfo(QString nodeId,bool & result){
    result = false;

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
    sUnit.dataUnit = package;
    sUnit.localServer = inServer;

    //[1]数据内容封装
    sUnit.dataUnit.data = package.data;

    //[2]添加数据传输协议头
    sUnit.dataUnit.wSourceAddr = package.wSourceAddr;
    sUnit.dataUnit.wDestAddr = package.wDestAddr;

    if(inServer){
        bool flag = false;
        OuterNetConfig destConfig = QueryNodeDescInfo(QString::number(package.wDestAddr),flag);
        if(flag){
            if(destConfig.communicationMethod == C_NetWork && destConfig.messageFormat == M_205){
                sUnit.method = C_UDP;
            }else if(destConfig.communicationMethod == C_TongKong && destConfig.messageFormat == M_495){
                sUnit.method = C_TCP;
            }
        }
    }else{
        bool findServer = false;
        NodeServer serverInfo = QueryServerDescInfo(QString::number(package.wDestAddr),findServer);
        if(findServer){
            if(serverInfo.communicationMethod == C_NetWork && serverInfo.messageFormat == M_205){
                sUnit.method = C_UDP;
            }else if(serverInfo.communicationMethod == C_TongKong && serverInfo.messageFormat == M_495){
                sUnit.method = C_TCP;
            }
        }
    }

    //[3]加入发送队列
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
