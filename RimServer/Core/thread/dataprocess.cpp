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

/*!
     * @brief 提供用户注册，返回注册的ID
     * @details  注册成功后，自动为用户创建一个默认的分组，并且名称命名为【我的好友】
     * @param[in] toolButton 待插入的工具按钮
     * @return 是否插入成功
     */
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

    SendData(data);
}

void DataProcess::processUserLogin(Database * db,int socketId, LoginRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    TcpClient * tmpClient = TcpClientManager::instance()->getClient(request->accountId);

    ResponseLogin loginResult = LOGIN_SUCCESS;

    if(tmpClient && tmpClient->getAccount().size() > 0)
    {
        loginResult = LOGIN_USER_LOGINED;
    }
    else
    {
        loginResult = RSingleton<SQLProcess>::instance()->processUserLogin(db,request);
    }

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

    SendData(data);
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

    SendData(data);
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

    SendData(data);
}

//TODO 找群时，先查找群主信息，查看是否在线
/*!
     * @brief 处理用户响应好友请求操作
     * @details A请求B，B向A回复结果。
     *          根据B的回复结果，若同意请求，则将对方的信息发送给对方，即发送A至B，发送B至A；
     *          若B拒绝请求，则直接将结果发送至A
     * @param[in] toolButton 待插入的工具按钮
     * @return 是否插入成功
     */
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

        delete ofresponse;
        SendData(reqeuestData);
    }
    else
    {
        result = RSingleton<SQLProcess>::instance()->processAddFriendRequest(db,request->accountId,request->operateId,(int)FRIEND_REQUEST);
    }

    responseData.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,result);

    delete request;

    SendData(responseData);
}

/*!
     * @brief 处理好友请求结果回复
     * @details A向B发出请求，B给A回复确认添加；
     *          1.在数据库中建立A、B两者之间的关系，即分别将对方加入己方的默认分组；
     *          2.若同意，则通知B，发送有关A的信息；若A在线，则将B的信息推送至A，若A离线，则将B确认的信息缓存；
     *          3.若不同意，则向A发送拒绝的结果；
     * @param[in] toolButton 待插入的工具按钮
     * @return 是否插入成功
     */
void DataProcess::processRelationOperate(Database *db, int socketId, OperateFriendRequest *request)
{
    bool flag = false;

    ResponseFriendApply result = (ResponseFriendApply)request->result;
    if(result == FRIEND_AGREE)
    {
        flag = RSingleton<SQLProcess>::instance()->establishRelation(db,request);
    }

    //【1】向自己发送对方联系人基本信息
    if(flag)
    {
        SocketOutData responseData;
        responseData.sockId = socketId;

        GroupingFriendResponse * responseA = new GroupingFriendResponse;
        responseA->type = G_Friend_CREATE;
        responseA->stype = request->stype;
        responseA->groupId = RSingleton<SQLProcess>::instance()->getDefaultGroupByUserAccountId(db,request->accountId);

        UserBaseInfo baseInfo;
        RSingleton<SQLProcess>::instance()->getUserInfo(db,request->operateId,baseInfo);
        responseA->user.accountId = baseInfo.accountId;
        responseA->user.nickName = baseInfo.nickName;
        responseA->user.signName = baseInfo.signName;
        responseA->user.face = baseInfo.face;
        responseA->user.customImgId = baseInfo.customImgId;
        responseA->user.remarks = baseInfo.nickName;

        responseData.data = RSingleton<MsgWrap>::instance()->handleMsg(responseA);
        delete responseA;
        SendData(responseData);
    }

    TcpClient * client = TcpClientManager::instance()->getClient(request->operateId);

    if(client && client->isOnLine())
    {
        //【2】向对方发送自己基本信息
        int operateSock = client->socket();
        SocketOutData reqeuestData;
        reqeuestData.sockId = operateSock;

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
        delete ofresponse;
        SendData(reqeuestData);

        if(result == FRIEND_AGREE && flag)
        {
            SocketOutData responseData;
            responseData.sockId = operateSock;

            GroupingFriendResponse * responseB = new GroupingFriendResponse;
            responseB->type = G_Friend_CREATE;
            responseB->stype = request->stype;
            responseB->groupId = RSingleton<SQLProcess>::instance()->getDefaultGroupByUserAccountId(db,request->operateId);

            UserBaseInfo baseInfo;
            RSingleton<SQLProcess>::instance()->getUserInfo(db,request->accountId,baseInfo);
            responseB->user.accountId = baseInfo.accountId;
            responseB->user.nickName = baseInfo.nickName;
            responseB->user.signName = baseInfo.signName;
            responseB->user.face = baseInfo.face;
            responseB->user.customImgId = baseInfo.customImgId;
            responseB->user.remarks = baseInfo.nickName;

            responseData.data = RSingleton<MsgWrap>::instance()->handleMsg(responseB);
            delete responseB;
            SendData(responseData);
        }
    }
    else
    {
        RSingleton<SQLProcess>::instance()->processAddFriendRequest(db,request->accountId,request->operateId,(int)request->result);
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
    delete response;

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
        delete response;
    }
    else
    {

    }

    delete request;

    SendData(responseData);
}

void DataProcess::processGroupingFriend(Database *db, int socketId, GroupingFriendRequest *request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    bool flag = false;

    switch(request->type)
    {
        case G_Friend_UPDATE:
            {
                flag = RSingleton<SQLProcess>::instance()->updateGroupFriendInfo(db,request);
            }
            break;
        case G_Friend_MOVE:
            {
                flag = RSingleton<SQLProcess>::instance()->updateMoveGroupFriend(db,request);
            }
            break;
        default:
            break;
    }

    GroupingFriendResponse * response = new GroupingFriendResponse();
    response->type = request->type;
    response->stype = request->stype;
    response->groupId = request->groupId;
    response->oldGroupId = request->oldGroupId;
    response->user = request->user;

    if(flag)
    {
        responseData.data = RSingleton<MsgWrap>::instance()->handleMsg(response,STATUS_SUCCESS);
    }
    else
    {
        responseData.data = RSingleton<MsgWrap>::instance()->handleMsg(response,STATUS_FAILE);
    }
    delete response;
    delete request;

    SendData(responseData);
}

void DataProcess::processText(Database *db, int socketId, TextRequest * request)
{
    SocketOutData responseData;

    TcpClient * client = TcpClientManager::instance()->getClient(request->destAccountId);
    if(client && client->isOnLine())
    {
        responseData.sockId = client->socket();

        TextResponse * response = new TextResponse;
        response->msgCommand = request->msgCommand;
        response->accountId = request->destAccountId;
        response->fromAccountId = request->accountId;
        response->sendData = request->sendData;
        response->timeStamp = request->timeStamp;

        responseData.data = RSingleton<MsgWrap>::instance()->handleText(response);

        SendData(responseData);
        delete response;
    }
    else
    {
        //TODO 保存信息至历史消息数据库
    }
    delete request;
}
