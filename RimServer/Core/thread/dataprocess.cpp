#include "dataprocess.h"

#include <QDebug>

#include "Util/rutil.h"
#include "rsingleton.h"
#include "sql/sqlprocess.h"
#include "Network/msgwrap.h"
#include "Network/head.h"
#include "Network/netglobal.h"

#include "Network/tcpclient.h"
using namespace ServerNetwork;

#define SendData(data) { G_SendMutex.lock();\
                         G_SendButts.enqueue(data);\
                         G_SendMutex.unlock();G_SendCondition.wakeOne();}

DataProcess::DataProcess()
{

}

void DataProcess::processUserRegist(Database *db, int socketId, RegistRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    QString registId,uuid;

    ResponseRegister regResult = RSingleton<SQLProcess>::instance()->processUserRegist(db,request,registId,uuid);

    if(regResult == REGISTER_SUCCESS)
    {
        RegistResponse * response = new RegistResponse;
        QString groupId;
        RSingleton<SQLProcess>::instance()->createGroup(db,uuid,QStringLiteral("我的好友"),groupId,true);
        response->accountId = registId;
        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);
        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,regResult);
    }

    delete request;

    SendData(data)
}

void DataProcess::processUserLogin(Database * db,int socketId, LoginRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    ResponseLogin loginResult = RSingleton<SQLProcess>::instance()->processUserLogin(db,request);

    if(loginResult == LOGIN_SUCCESS)
    {
        LoginResponse * response = new LoginResponse;
        RSingleton<SQLProcess>::instance()->getUserInfo(db,request->accountId,response->baseInfo);
        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);

        TcpClient * client = TcpClientManager::instance()->getClient(socketId);
        if(client)
        {
            client->setAccount(request->accountId);
            client->setNickName(response->baseInfo.nickName);
            client->setOnLine(true);
            client->setOnLineState((int)request->status);
        }
        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,loginResult);
    }

    delete request;

    SendData(data)
}

void DataProcess::processUpdateUserInfo(Database * db,int socketId, UpdateBaseInfoRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    ResponseUpdateUser updateResult = RSingleton<SQLProcess>::instance()->processUpdateUserInfo(db,request);

    if(updateResult == UPDATE_USER_SUCCESS)
    {

        UpdateBaseInfoResponse * response = new UpdateBaseInfoResponse;

        RSingleton<SQLProcess>::instance()->getUserInfo(db,request->baseInfo.accountId,response->baseInfo);
        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);

        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,updateResult);
    }

    delete request;

    SendData(data)
}

void DataProcess::processSearchFriend(Database * db,int socketId, SearchFriendRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;
    SearchFriendResponse * response = new SearchFriendResponse;

    ResponseAddFriend updateResult = RSingleton<SQLProcess>::instance()->processSearchFriend(db,request,response);

    if(updateResult == FIND_FRIEND_FOUND)
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);
        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,updateResult);
    }

    delete request;

    SendData(data)
}

//TODO 找群时，先查找群主信息，查看是否在线
void DataProcess::processAddFriend(Database * db,int socketId, AddFriendRequest *request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    ResponseAddFriend result = ADD_FRIEND_SENDED;

    TcpClient * client = TcpClientManager::instance()->getClient(request->operateId);
    if(client && client->isOnLine())
    {
        SocketOutData reqeuestData;
        reqeuestData.sockId = client->socket();

        OperateFriendResponse * ofresponse = new OperateFriendResponse();
        ofresponse->type = FRIEND_APPLY;
        ofresponse->result = (int)FRIEND_REQUEST;
        ofresponse->stype = request->stype;
        ofresponse->accountId = client->getAccount();

        UserBaseInfo baseInfo;
        RSingleton<SQLProcess>::instance()->getUserInfo(db,request->accountId,baseInfo);
        ofresponse->requestInfo.accountId = baseInfo.accountId;
        ofresponse->requestInfo.nickName = baseInfo.nickName;
        ofresponse->requestInfo.signName = baseInfo.signName;
        ofresponse->requestInfo.face = baseInfo.face;
        ofresponse->requestInfo.customImgId = baseInfo.customImgId;

        reqeuestData.data = RSingleton<MsgWrap>::instance()->handleMsg(ofresponse);

        SendData(reqeuestData)
    }
    else
    {
        result = RSingleton<SQLProcess>::instance()->processAddFriendRequest(db,request->accountId,request->operateId,(int)FRIEND_REQUEST);
    }

    responseData.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,result);

    delete request;

    SendData(responseData)
}

//处理好友请求操作,包括同意或者拒绝
void DataProcess::processRelationOperate(Database *db, int socketId, OperateFriendRequest *request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    bool flag = false;

    ResponseFriendApply result = (ResponseFriendApply)request->result;
    if(result == FRIEND_AGREE)
    {
       flag = RSingleton<SQLProcess>::instance()->establishRelation(db,request);
    }

    if(flag)
    {
        TcpClient * client = TcpClientManager::instance()->getClient(request->operateId);

        if(client && client->isOnLine())
        {
            SocketOutData reqeuestData;
            reqeuestData.sockId = client->socket();

            OperateFriendResponse * ofresponse = new OperateFriendResponse();
            ofresponse->type = FRIEND_APPLY;
            ofresponse->result = (int)request->result;
            ofresponse->stype = request->stype;
            ofresponse->accountId = client->getAccount();

            UserBaseInfo baseInfo;
            RSingleton<SQLProcess>::instance()->getUserInfo(db,request->accountId,baseInfo);
            ofresponse->requestInfo.accountId = baseInfo.accountId;
            ofresponse->requestInfo.nickName = baseInfo.nickName;
            ofresponse->requestInfo.signName = baseInfo.signName;
            ofresponse->requestInfo.face = baseInfo.face;
            ofresponse->requestInfo.customImgId = baseInfo.customImgId;

            reqeuestData.data = RSingleton<MsgWrap>::instance()->handleMsg(ofresponse);

            SendData(reqeuestData)
        }
        else
        {
            RSingleton<SQLProcess>::instance()->processAddFriendRequest(db,request->accountId,request->operateId,(int)request->result);
        }
    }

    delete request;
}

void DataProcess::processFriendList(Database *db, int socketId, FriendListRequest *request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    FriendListResponse * response = new FriendListResponse;
    response->accountId = request->accountId;
    bool flag = RSingleton<SQLProcess>::instance()->getFriendList(db,request->accountId,response);
    if(flag)
    {
        responseData.data = RSingleton<MsgWrap>::instance()->handleMsg(response);
    }
    else
    {
        responseData.data = RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,(int)STATUS_FAILE);
    }

    delete request;

    SendData(responseData);
}

void DataProcess::processGroupingOperate(Database *db, int socketId, GroupingRequest *request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    bool flag = false;
    QString groupId = request->groupId;

    switch(request->type)
    {
        case GROUPING_CREATE:
                            {
                               flag = RSingleton<SQLProcess>::instance()->createGroup(db,request->uuid,request->groupName,groupId);
                            }
                            break;
        case GROUPING_RENAME:
                            {
                               flag = RSingleton<SQLProcess>::instance()->renameGroup(db,request);
                            }
                            break;
        case GROUPING_DELETE:
                            {
                               flag = RSingleton<SQLProcess>::instance()->deleteGroup(db,request);
                            }
                            break;
        case GROUPING_SORT:
                            {
                                //RSingleton<SQLProcess>::instance()->createGroup(db,socketId,request->groupName);
                            }
                            break;
        default:
            break;
    }

    if(flag)
    {
        GroupingResponse * response = new GroupingResponse;
        response->uuid = request->uuid;
        response->gtype = request->gtype;
        response->type = request->type;
        response->groupId = groupId;

        responseData.data = RSingleton<MsgWrap>::instance()->handleMsg(response);
    }
    else
    {

    }

    delete request;

    SendData(responseData);
}
