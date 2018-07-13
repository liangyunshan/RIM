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

#include "protocol/datastruct.h"
#include "protocol/protocoldata.h"
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
    void onRecvFriendList(MsgOperateResponse status, FriendListResponse * response);
    void onRecvFriendGroupingOperate(MsgOperateResponse status, GroupingResponse response);
    void onRecvGroupGroupingOperate(MsgOperateResponse status,GroupingResponse response);
    void onErrorGroupingOperate(OperateGrouping type);
    void onRecvSearchChatroomResponse(ResponseAddFriend result,SearchGroupResponse response);
    void onRecvGroupList(MsgOperateResponse status,ChatGroupListResponse * response);
    void onRecvResitGroup(MsgOperateResponse status, RegistGroupResponse response);
    void onRecvOpreateGroup(MsgOperateResponse status, GroupingChatResponse response);
    void onRecvGroupCommand(MsgOperateResponse status, GroupingCommandResponse response);
    void onRecvText(TextRequest response);
    void onRecvTextReply(TextReply reply);
    void onRecvGroupingFriend(MsgOperateResponse result,GroupingFriendResponse response);
    void onRecvFileControl(SimpleFileItemRequest request);
    void onRecvFileRequest(FileItemRequest request);
    void onRecvFileData(QString fileId,QString fileName);
    void onScreenChanged();
    void onAddHistoryItem(HistoryChatRecord &record);
    void onTransmitsInitialError(QString errorMsg);
    void onTextConnected(bool flag);
    void onTextSocketError();
    void onFileConnected(bool flag);
    void onFileSocketError();
    void onFileTransStatusChanged(FileTransProgress progress);

signals:
    void recvRegistResponse(ResponseRegister status,RegistResponse response);
    void recvLoginResponse(ResponseLogin status,LoginResponse response);
    void recvUpdateBaseInfoResponse(ResponseUpdateUser status,UpdateBaseInfoResponse response);
    void recvUserStateChangedResponse(MsgOperateResponse result,UserStateResponse response);
    void recvSearchFriendResponse(ResponseAddFriend status, SearchFriendResponse response);
    void recvAddFriendResponse(ResponseAddFriend);
    void recvFriendRequest(OperateFriendResponse response);
    void recvFriendList(MsgOperateResponse status,FriendListResponse * response);
    void recvFriendGroupingOperate(MsgOperateResponse status,GroupingResponse response);
    void recvGroupGroupingOperate(MsgOperateResponse status,GroupingResponse response);
    void errorGroupingOperate(OperateGrouping type);

    void recvGroupList(MsgOperateResponse status,ChatGroupListResponse * response);
    void recvRegistGroup(MsgOperateResponse status,RegistGroupResponse response);
    void recvGroupItemOperate(MsgOperateResponse result,GroupingChatResponse response);
    void recvRegistGroupFailed();
    void recvGroupCommand(MsgOperateResponse result,GroupingCommandResponse response);

    void recvText(TextRequest request);
    void recvTextReply(TextReply);
    void recvSearchChatroomResponse(ResponseAddFriend result,SearchGroupResponse response);
    void recvRelationFriend(MsgOperateResponse result,GroupingFriendResponse response);

    void recvFileControl(SimpleFileItemRequest control);
    void recvFileRequest(FileItemRequest control);
    void recvFileData(QString fileId,QString fileName);

    void screenChange();

    void createHisotryItem(HistoryChatRecord);

    void transmitsInitialError(QString error);

    void textConnected(bool);
    void textSocketError();
    void fileConnected(bool);
    void fileSocketError();
    void sigTransStatus(FileTransProgress);

private:
    MessDiapatch(QObject *parent = 0);
    static MessDiapatch * dispatch;
};

#endif // MESSDIAPATCH_H
