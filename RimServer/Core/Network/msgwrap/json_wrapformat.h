/*!
 *  @brief     内部数据传输封装
 *  @details   将protocoldata中的结构体，按照json格式封装
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.25
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef JSON_WRAPFORMAT_H
#define JSON_WRAPFORMAT_H

#include "msgwrap.h"

class Json_WrapFormat : public MsgWrap
{
public:
    explicit Json_WrapFormat();

    QByteArray handleMsg(MsgPacket * packet, int result = 0
#ifdef __LOCAL_CONTACT__
                           ,CommucationMethod method = C_None,MessageFormat format = M_NONE
#endif
            );

    QByteArray handleMsgReply(MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1);

private:
    QByteArray handleRegistResponse(RegistResponse * packet);
    QByteArray handleLoginResponse(LoginResponse *packet);
    QByteArray handleUpdateBaseInfoResponse(UpdateBaseInfoResponse * packet);
    QByteArray handleUserStateChanged(UserStateResponse * packet);

    QByteArray handleSearchFriendResponse(SearchFriendResponse * packet);
    QByteArray handleOperateFriendResponse(OperateFriendResponse * packet);
    QByteArray handleFriendListResponse(FriendListResponse *packet, int result);
    QByteArray handleGroupingResponse(GroupingResponse *packet, int result);
    QByteArray handleGroupingFriend(GroupingFriendResponse *packet, int result);

    QByteArray handleGroupList(ChatGroupListResponse * packet,int result);
    QByteArray handleCreateGroup(RegistGroupResponse *packet,int result);
    QByteArray handleSearchGroup(SearchGroupResponse *packet,int result);
    QByteArray handleOpreateGroup(GroupingChatResponse *packet,int result);
    QByteArray handleOpreateCommand(GroupingCommandResponse *packet,int result);

    QByteArray handleText(TextRequest * request);
    QByteArray handleTextReply(TextReply * response);

    QByteArray wrappedPack(MsgPacket * packet, int status, QJsonArray &data);
    QByteArray wrappedPack(MsgPacket * packet,  int status, QJsonObject &data);

};

#endif // JSON_WRAPFORMAT_H
