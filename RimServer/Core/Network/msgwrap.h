/*!
 *  @brief     网络数据转换
 *  @details   接收应用层的数据，将其转换为网络数据发送结构
 *  @file      msgwrap.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.15
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGWRAP_H
#define MSGWRAP_H

#include <QJsonObject>
#include <QJsonArray>

#include "protocoldata.h"
using namespace ProtocolType;

class MsgWrap
{
public:
    MsgWrap();

    QByteArray handleMsg(MsgPacket * packet, int result = 0);
    QByteArray handleText(TextRequest * request);
    QByteArray handleTextReply(TextReply * response);
    QByteArray handleMsgReply(MsgType type,MsgCommand command,int replyCode,int subMsgCommand = -1);

    QByteArray handleFile(MsgPacket *response);

    QByteArray handleFileData(QString fileMd5,size_t currIndex,QByteArray array);

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

    QByteArray handleFileControl(SimpleFileItemRequest * packet);
    QByteArray handleFileRequest(FileItemRequest * packet);

    QByteArray wrappedPack(MsgPacket * packet,  int status, QJsonObject &data);
    QByteArray wrappedPack(MsgPacket * packet, int status, QJsonArray &data);

};

#endif // MSGWRAP_H
