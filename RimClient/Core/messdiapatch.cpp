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

void MessDiapatch::onRecvFriendList(FriendListResponse *response)
{
    emit recvFriendList(response);
}

void MessDiapatch::onRecvGroupingOperate(GroupingResponse response)
{
    emit recvGroupingOperate(response);
}

void MessDiapatch::onErrorGroupingOperate(OperateGrouping type)
{
    emit errorGroupingOperate(type);
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
    if(response.stype == SearchPerson)
    {
        emit recvRelationFriend(result,response);
    }
    else if(response.type == SearchGroup)
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

void MessDiapatch::onScreenChanged()
{
    emit screenChange();
}
