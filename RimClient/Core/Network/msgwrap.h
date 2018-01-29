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

#include "protocoldata.h"
using namespace ProtocolType;

class MsgWrap
{
public:
    MsgWrap();

    void handleMsg(MsgPacket * packet);

private:
    void handleRegistRequest(RegistRequest *packet);
    void handleLoginRequest(LoginRequest *packet);
    void handleUpdateBaseInfoRequest(UpdateBaseInfoRequest * packet);
    void handleSearchFriendRequest(SearchFriendRequest * packet);
    void handleAddFriendRequest(AddFriendRequest * packet);
    void handleOperateFriendRequest(OperateFriendRequest * packet);
    void handleFriendListRequest(FriendListRequest *packet);

    void wrappedPack(MsgPacket * packet, QJsonObject &data);

private:

};

#endif // MSGWRAP_H
