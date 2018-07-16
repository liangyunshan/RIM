#include "messdiapatch.h"

MessDiapatch * MessDiapatch::dispatch = NULL;

MessDiapatch::MessDiapatch(QObject *parent) : QObject(parent)
{

}

MessDiapatch *MessDiapatch::instance()
{
    if(dispatch == NULL)
    {
        dispatch = new MessDiapatch();
    }

    return dispatch;
}

void MessDiapatch::onRecvRegistResponse(ResponseRegister status, RegistResponse response)
{
    emit recvRegistResponse(status,response);
}

void MessDiapatch::onRecvLoginResponse(ResponseLogin status, LoginResponse response)
{
    emit recvLoginResponse(status,response);
}

void MessDiapatch::onRecvUpdateBaseInfoResponse(ResponseUpdateUser status,UpdateBaseInfoResponse response)
{
    emit recvUpdateBaseInfoResponse(status,response);
}

void MessDiapatch::onRecvUserStateChangedResponse(MsgOperateResponse result, UserStateResponse response)
{
    emit recvUserStateChangedResponse(result,response);
}

void MessDiapatch::onRecvSearchFriendResponse(ResponseAddFriend status, SearchFriendResponse response)
{
    emit recvSearchFriendResponse(status,response);
}

void MessDiapatch::onRecvAddFriendResponse(ResponseAddFriend status)
{
    emit recvAddFriendResponse(status);
}

void MessDiapatch::onRecvFriendRequest(OperateFriendResponse response)
{
    emit recvFriendRequest(response);
}

void MessDiapatch::onRecvFriendList(MsgOperateResponse status,FriendListResponse *response)
{
    emit recvFriendList(status,response);
}

void MessDiapatch::onRecvFriendGroupingOperate(MsgOperateResponse status,GroupingResponse response)
{
    emit recvFriendGroupingOperate(status,response);
}

void MessDiapatch::onRecvGroupGroupingOperate(MsgOperateResponse status, GroupingResponse response)
{
    emit recvGroupGroupingOperate(status,response);
}

void MessDiapatch::onErrorGroupingOperate(OperateGrouping type)
{
    emit errorGroupingOperate(type);
}

void MessDiapatch::onRecvSearchChatroomResponse(ResponseAddFriend result, SearchGroupResponse response)
{
    emit recvSearchChatroomResponse(result,response);
}

void MessDiapatch::onRecvGroupList(MsgOperateResponse status, ChatGroupListResponse *response)
{
    emit recvGroupList(status,response);
}

void MessDiapatch::onRecvResitGroup(MsgOperateResponse status,RegistGroupResponse response)
{
    emit recvRegistGroup(status,response);
}

void MessDiapatch::onRecvOpreateGroup(MsgOperateResponse status , GroupingChatResponse response)
{
    emit recvGroupItemOperate(status,response);
}

void MessDiapatch::onRecvGroupCommand(MsgOperateResponse status, GroupingCommandResponse response)
{
    emit recvGroupCommand(status,response);
}

void MessDiapatch::onRecvText(TextRequest response)
{
    emit recvText(response);
}

void MessDiapatch::onRecvTextReply(TextReply reply)
{
    emit recvTextReply(reply);
}

void MessDiapatch::onRecvGroupingFriend(MsgOperateResponse result, GroupingFriendResponse response)
{
    if(response.stype == OperatePerson)
    {
        emit recvRelationFriend(result,response);
    }
    else if(response.type == OperateGroup)
    {

    }
}

void MessDiapatch::onRecvFileControl(SimpleFileItemRequest request)
{
    emit recvFileControl(request);
}

void MessDiapatch::onRecvFileRequest(FileItemRequest request)
{
    emit recvFileRequest(request);
}

void MessDiapatch::onRecvFileData(QString fileId,QString fileName)
{
    emit recvFileData(fileId,fileName);
}

void MessDiapatch::onRecvFileData(FileRecvDesc desc)
{
    emit recvFileData(desc);
}

void MessDiapatch::onScreenChanged()
{
    emit screenChange();
}

void MessDiapatch::onAddHistoryItem(HistoryChatRecord &record)
{
    emit createHisotryItem(record);
}

void MessDiapatch::onTransmitsInitialError(QString errorMsg)
{
    emit transmitsInitialError(errorMsg);
}

void MessDiapatch::onTextConnected(bool flag)
{
    emit textConnected(flag);
}

void MessDiapatch::onTextSocketError()
{
    emit textSocketError();
}

void MessDiapatch::onFileConnected(bool flag)
{
    emit fileConnected(flag);
}

void MessDiapatch::onFileSocketError()
{
    emit fileSocketError();
}

/*!
 * @brief 文件传输状态改变
 * @param progress 文件传输状态
 */
void MessDiapatch::onFileTransStatusChanged(FileTransProgress progress)
{
    emit sigTransStatus(progress);
}
