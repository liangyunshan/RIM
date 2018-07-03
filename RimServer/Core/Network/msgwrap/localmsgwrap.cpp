#include "localmsgwrap.h"

#ifdef __LOCAL_CONTACT__

#include "Network/head.h"
#include "Network/netglobal.h"
#include "binary716_wrapfromat.h"
#include "../wraprule/qdb21_wraprule.h"
#include "../wraprule/qdb2051_wraprule.h"
#include "../wraprule/udp_wraprule.h"
#include "../wraprule/tcp_wraprule.h"
#include "rsingleton.h"
#include "global.h"
#include <QDebug>

#include "../../protocol/datastruct.h"
using namespace ParameterSettings;

std::mutex QueryNodeMutex;

/*!
 * @brief 根据节点查找对应节点的相信通信配置信息
 * @param[in] nodeId 目的节点号
 * @param[in] result 查找结果，true表示查找成功，false表示查找失败
 * @return 所查找的节点号信息
 */
OuterNetConfig queryNodeDescInfo(QString nodeId,bool & result){
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
    return OuterNetConfig();
}


LocalMsgWrap::LocalMsgWrap():MsgWrap()
{

}

void LocalMsgWrap::hanldeMsgProtol(int sockId,ProtocolPackage & package)
{
    SendUnit sUnit;
    sUnit.sockId = sockId;
    sUnit.method = C_NONE;
    sUnit.dataUnit = package;

    //[1]数据内容封装
    sUnit.dataUnit.data = package.data;

    //[2]添加数据传输协议头
    sUnit.dataUnit.wSourceAddr = package.wSourceAddr;
    sUnit.dataUnit.wDestAddr = package.wDestAddr;

    bool flag = false;
    OuterNetConfig destConfig = queryNodeDescInfo(QString::number(package.wDestAddr),flag);
    if(flag){
        if(destConfig.communicationMethod == C_NetWork && destConfig.messageFormat == M_205){
            sUnit.method = C_UDP;
            RSingleton<UDP_WrapRule>::instance()->wrap(package);
        }else if(destConfig.communicationMethod == C_TongKong && destConfig.messageFormat == M_495){
            sUnit.method = C_TCP;
            RSingleton<TCP_WrapRule>::instance()->wrap(sUnit.dataUnit);
        }
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
               <<"sUnit.dataUnit.usSerialNo:"<<sUnit.dataUnit.usSerialNo
              <<"\n";

        //[3]加入发送队列
        if(sUnit.method != C_NONE){
            std::unique_lock<std::mutex> ul(G_SendMutex);
            G_SendButts.push(sUnit);
            G_SendCondition.notify_one();
        }
    }
}

void LocalMsgWrap::handleMsgReply(int sockId, MsgType type, MsgCommand command, int replyCode, int subMsgCommand)
{

}

#endif
