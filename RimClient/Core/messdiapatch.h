/*!
 *  @brief     信息中转
 *  @details   将下层处理的结果，通过此类分发至具体的结果页面
 *  @file      messdiapatch.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.XX
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MESSDIAPATCH_H
#define MESSDIAPATCH_H

#include <QObject>

#include "protocoldata.h"
using namespace ProtocolType;

class MessDiapatch : public QObject
{
    Q_OBJECT
public:
    static MessDiapatch * instance();

    void onRecvRegistResponse(ResponseRegister status,RegistResponse response);
    void onRecvLoginResponse(ResponseLogin status, LoginResponse response);
    void onRecvUpdateBaseInfoResponse(ResponseUpdateUser status, UpdateBaseInfoResponse response);
    void onRecvUserStateChangedResponse(MsgOperateResponse result,UserStateResponse response);

    void onRecvSearchFriendResponse(ResponseAddFriend status, SearchFriendResponse response);
    void onRecvAddFriendResponse(ResponseAddFriend status);
    void onRecvFriendRequest(OperateFriendResponse response);
    void onRecvFriendList(FriendListResponse * response);
    void onRecvGroupingOperate(GroupingResponse response);
    void onErrorGroupingOperate(OperateGrouping type);
    void onRecvText(TextRequest response);
    void onRecvTextReply(TextReply reply);
    void onRecvGroupingFriend(MsgOperateResponse result,GroupingFriendResponse response);

    void onRecvFileControl(SimpleFileItemRequest request);

    void onScreenChanged();

signals:
    void recvRegistResponse(ResponseRegister status,RegistResponse response);
    void recvLoginResponse(ResponseLogin status,LoginResponse response);
    void recvUpdateBaseInfoResponse(ResponseUpdateUser status,UpdateBaseInfoResponse response);
    void recvUserStateChangedResponse(MsgOperateResponse result,UserStateResponse response);
    void recvSearchFriendResponse(ResponseAddFriend status, SearchFriendResponse response);
    void recvAddFriendResponse(ResponseAddFriend);
    void recvFriendRequest(OperateFriendResponse response);
    void recvFriendList(FriendListResponse * response);
    void recvGroupingOperate(GroupingResponse response);
    void errorGroupingOperate(OperateGrouping type);
    void recvText(TextRequest request);
    void recvTextReply(TextReply);
    void recvRelationFriend(MsgOperateResponse result,GroupingFriendResponse response);

    void recvFileControl(SimpleFileItemRequest control);

    void screenChange();

private:
    MessDiapatch(QObject *parent = 0);
    static MessDiapatch * dispatch;
};

#endif // MESSDIAPATCH_H
