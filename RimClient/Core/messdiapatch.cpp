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

void MessDiapatch::onRecvSearchFriendResponse(ResponseAddFriend status, SearchFriendResponse response)
{
    emit recvSearchFriendResponse(status,response);
}

void MessDiapatch::onRecvAddFriendResponse(ResponseAddFriend status)
{
    emit recvAddFriendResponse(status);
}
