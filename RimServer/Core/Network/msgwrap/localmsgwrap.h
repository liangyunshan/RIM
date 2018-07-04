/*!
 *  @brief     本地信息包装
 *  @details   实现716通信协议下的报文封装
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.28
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note       20180702:wey:增加服务器-服务器数据封装
 */
#ifndef LOCALMSGWRAP_H
#define LOCALMSGWRAP_H

#ifdef __LOCAL_CONTACT__

#include <map>
#include <vector>
#include <mutex>

#include <QString>

#include "../../protocol/protocoldata.h"
using namespace ProtocolType;

#include "../../protocol/datastruct.h"
#include "msgwrap.h"

class NetConnector;

class LocalMsgWrap : public MsgWrap
{
public:
    LocalMsgWrap();

    /*!
     *  @brief  建立连接过程状态
     *  @note 未建立-建立过程中-建立成功/失败
     */
    enum ServerConnStats{
        SCS_NONE,       /*!< 未建立 */
        SCS_OK,         /*!< 建立成功 */
        SCS_ERR,        /*!< 建立失败 */
        SCS_IN          /*!< 建立过程中 */
    };

    struct ServerCacheInfo{
        ServerCacheInfo():connStats(SCS_NONE){

        }
        ServerConnStats connStats;
        ParameterSettings::NodeServer serverInfo;
        std::vector<ProtocolPackage> msgCache;
    };

    void handleMsg(int sockId ,MsgPacket * packet, int result = 0){}
    void hanldeMsgProtcol(int sockId, ProtocolPackage & package,bool inServer = true);

    void cacheMsgProtocol(ParameterSettings::NodeServer serverInfo, ProtocolPackage & package);

    void handleMsgReply(int sockId ,MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1);

private:
    void respNetConnectResult(QString nodeId, bool connected, int socketId);

private:
    std::mutex cacheMutex;
    std::map<QString,ServerCacheInfo> serverCache;
};

#endif

#endif // LOCALMSGWRAP_H
