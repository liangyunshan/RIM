/*!
 *  @brief     内部数据传输封装
 *  @details   将protocoldata中的结构体，按照json格式封装
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.08
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef JSON_WRAPFORMAT_H
#define JSON_WRAPFORMAT_H

#include <QJsonObject>

#include "wrapformat.h"

using namespace ProtocolType;

class Json_WrapFormat : public WrapFormat
{
public:
    Json_WrapFormat();

    void handleMsg(MsgPacket * packet,QByteArray & result);

private:
    //文本信息
    void handleTextRequest(TextRequest *packet, QByteArray &result);

    //状态控制信息
    void handleRegistRequest(RegistRequest *packet,QByteArray & result);
    void handleLoginRequest(LoginRequest *packet,QByteArray & result);
    void handleUpdateBaseInfoRequest(UpdateBaseInfoRequest * packet,QByteArray & result);
    void handleUserStateRequest(UserStateRequest * request,QByteArray & result);
    void handleSearchFriendRequest(SearchFriendRequest * packet,QByteArray & result);
    void handleAddFriendRequest(AddFriendRequest * packet,QByteArray & result);
    void handleOperateFriendRequest(OperateFriendRequest * packet,QByteArray & result);
    void handleFriendListRequest(FriendListRequest *packet,QByteArray & result);
    void handleGroupingOperateRequest(GroupingRequest *packet,QByteArray & result);
    void handleGroupingFriendRequest(GroupingFriendRequest * packet,QByteArray & result);

    void handleChatGroupListRequest(ChatGroupListRequest * packet,QByteArray & result);
    void handleRegistGroupRequest( RegistGroupRequest * packet,QByteArray & result);
    void handleGroupCommandRequest(GroupingCommandRequest *packet,QByteArray & result);
    void handleHistoryMsgRequest(HistoryMessRequest *packet,QByteArray & result);

    void wrappedPack(MsgPacket *packet,QJsonObject & data,QByteArray& result);
};

#endif // JSON_MSGWRAP_H
