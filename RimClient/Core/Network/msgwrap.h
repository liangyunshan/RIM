/*!
 *  @brief     网络数据转换
 *  @details   接收应用层的数据，将其转换为网络数据发送结构 @n
 *             1.文本传输，采用JSON数据格式 @n
 *             2.文件传输，采用流传输 @n
 *  @file      msgwrap.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20170128:wey:添加联系人请求
 *      20170129:wey:添加好友列表请求
 *      20170130:wey:添加分组操作请求
 */
#ifndef MSGWRAP_H
#define MSGWRAP_H

#include <QJsonObject>

#include "protocoldata.h"
using namespace ProtocolType;

class MsgWrap
{
public:
    MsgWrap();

    void handleMsg(MsgPacket * packet);
    void hanleText(TextRequest *packet);

    void handelFileControl(SimpleFileItemRequest * request);
    void handleFileRequest(FileItemRequest * fileRequest);
    void handleFileData(QString fileId, size_t currIndex, QByteArray array);

private:
    void handleRegistRequest(RegistRequest *packet);
    void handleLoginRequest(LoginRequest *packet);
    void handleUpdateBaseInfoRequest(UpdateBaseInfoRequest * packet);
    void handleUserStateRequest(UserStateRequest * request);
    void handleSearchFriendRequest(SearchFriendRequest * packet);
    void handleAddFriendRequest(AddFriendRequest * packet);
    void handleOperateFriendRequest(OperateFriendRequest * packet);
    void handleFriendListRequest(FriendListRequest *packet);
    void handleGroupingOperateRequest(GroupingRequest *packet);
    void handleGroupingFriendRequest(GroupingFriendRequest * packet);

    void handleChatGroupListRequest(ChatGroupListRequest * packet);
    void handleRegistGroupRequest( RegistGroupRequest * packet);

    void wrappedPack(MsgPacket * packet, QJsonObject &data);

private:

};

#endif // MSGWRAP_H
