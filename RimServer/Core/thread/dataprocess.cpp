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

/*!
 * @brief 响应用户登陆，并返回登陆结果
 * @param[in] db 数据库
 * @param[in] socketId 网络标识
 * @param[in] request 登陆请求
 * @return 无
 * @note 具体的处理流程: @n
 *      1.响应用户登陆状态结果; @n
 *      2.向用户的在线好友推送用户的登陆状态; @n
 *
 */
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
            client->setOnLineState((int)request->status);
        }
        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,loginResult);
    }

    SendData(data);

    //[1]向用户对应的好友推送上线通知。
    pushFriendMyStatus(db,socketId,request->accountId,request->status);

    delete request;
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

void DataProcess::processUserStateChanged(Database *db, int socketId, UserStateRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    TcpClient * client = TcpClientManager::instance()->getClient(request->accountId);
    if(client)
    {
        client->setOnLineState((int)request->onStatus);

        UserStateResponse * response = new UserStateResponse;
        response->accountId = request->accountId;
        response->onStatus = request->onStatus;

        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);

        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,STATUS_FAILE);
    }

    SendData(data);

    pushFriendMyStatus(db,socketId,request->accountId,request->onStatus);

    delete request;
}

/*!
 * @brief 向好友推送当前自己的状态信息
 * @param[in] db 数据库
 * @param[in] socketId 网络标识
 * @param[in] accountId 用户自己ID
 * @param[in] onStatus 用户登陆状态
 * @return 无
 */
void DataProcess::pushFriendMyStatus(Database *db, int socketId, QString accountId,OnlineStatus onStatus)
{
    QList<QString> friendList;
    RSingleton<SQLProcess>::instance()->getFriendAccountList(db,accountId,friendList);

    QList<QString>::iterator iter = friendList.begin();
    while(iter != friendList.end())
    {
        TcpClient * friendClient = TcpClientManager::instance()->getClient((*iter));
        if(friendClient)
        {
            SocketOutData tmpData;
            tmpData.sockId = friendClient->socket();

            UserStateResponse * response = new UserStateResponse;
            response->accountId = accountId;
            response->onStatus = onStatus;

            tmpData.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);

            delete response;

            SendData(tmpData);
        }
        iter++;
    }
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
    if(client && ((OnlineStatus)client->getOnLineState() != STATUS_OFFLINE) )
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

        TcpClient * operateClient = TcpClientManager::instance()->getClient(request->operateId);
        if(operateClient)
        {
            responseA->user.status = (OnlineStatus)operateClient->getOnLineState();
        }
        else
        {
            responseA->user.status = STATUS_OFFLINE;
        }

        responseData.data = RSingleton<MsgWrap>::instance()->handleMsg(responseA);
        delete responseA;
        SendData(responseData);
    }

    TcpClient * client = TcpClientManager::instance()->getClient(request->operateId);

    if(client && ((OnlineStatus)client->getOnLineState() != STATUS_OFFLINE) )
    {
        //【2】向对方发送此次好友请求的处理结果信息
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

        //【3】若同意请求，则再次向对方发送自己的基本信息
        if(result == FRIEND_AGREE && flag)
        {
            SocketOutData responseData;
            responseData.sockId = operateSock;

            GroupingFriendResponse * responseB = new GroupingFriendResponse;
            responseB->type = G_Friend_CREATE;
            responseB->stype = request->stype;
            responseB->groupId = RSingleton<SQLProcess>::instance()->getDefaultGroupByUserAccountId(db,request->operateId);

            responseB->user.accountId = baseInfo.accountId;
            responseB->user.nickName = baseInfo.nickName;
            responseB->user.signName = baseInfo.signName;
            responseB->user.face = baseInfo.face;
            responseB->user.customImgId = baseInfo.customImgId;
            responseB->user.remarks = baseInfo.nickName;

            TcpClient * operateClient = TcpClientManager::instance()->getClient(request->accountId);
            if(operateClient)
            {
                responseB->user.status = (OnlineStatus)operateClient->getOnLineState();
            }
            else
            {
                responseB->user.status = STATUS_OFFLINE;
            }

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

/*!
 * @brief 获取好友用户列表
 * @details 按照当前用户分组，将好友组装成对应的列表存储。同时拉取对应的历史消息，一并推送至用户。
 * @param[in] db 数据库ID
 * @param[in] socketId 网络访问标识
 * @param[in] socketId 列表请求
 * @return 无
 * @note 具体的处理流程: @n
 *      1.获取用户对应的好友信息，并按照分组排列组装好; @n
 *      2.抓取当前好友登陆状态; @n
 *      3.推送当前用户历史系统消息; @n
 *      4.推送当前用户离线消息; @n
 * @attention 若可以查到好友在线状态，则将当前状态作为好友状态；若查找不到，默认使用离线状态表示。
 */
void DataProcess::processFriendList(Database *db, int socketId, FriendListRequest *request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    FriendListResponse * response = new FriendListResponse;
    response->accountId = request->accountId;
    bool flag = RSingleton<SQLProcess>::instance()->getFriendList(db,request->accountId,response);
    if(flag)
    {
        for(int i = 0; i < response->groups.size(); i++)
        {
            QList<SimpleUserInfo *>::iterator iter = response->groups.at(i)->users.begin();
            while(iter != response->groups.at(i)->users.end())
            {
                TcpClient * client = TcpClientManager::instance()->getClient((*iter)->accountId);
                if(client)
                {
                    (*iter)->status = (OnlineStatus)client->getOnLineState();
                }
                else
                {
                    (*iter)->status = STATUS_OFFLINE;
                }
                iter++;
            }
        }

        responseData.data = RSingleton<MsgWrap>::instance()->handleMsg(response);
    }
    else
    {
        responseData.data = RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,(int)STATUS_FAILE);
    }

    SendData(responseData);

    //推送历史消息
    {
        //[1]查找该用户对应的系统消息
        QList<AddFriendRequest> systemRequest;
        if(RSingleton<SQLProcess>::instance()->loadSystemCache(db,request->accountId,systemRequest))
        {
            QList<AddFriendRequest>::iterator iter = systemRequest.begin();
            while(iter != systemRequest.end())
            {
                SocketOutData reqeuestData;
                reqeuestData.sockId = socketId;

                OperateFriendResponse * ofresponse = new OperateFriendResponse();
                ofresponse->type = FRIEND_APPLY;
                ofresponse->result = (int)FRIEND_REQUEST;
                ofresponse->stype = (*iter).stype;
                ofresponse->accountId = request->accountId;

                UserBaseInfo baseInfo;
                RSingleton<SQLProcess>::instance()->getUserInfo(db,(*iter).accountId,baseInfo);
                ofresponse->requestInfo.accountId = baseInfo.accountId;
                ofresponse->requestInfo.nickName = baseInfo.nickName;
                ofresponse->requestInfo.signName = baseInfo.signName;
                ofresponse->requestInfo.face = baseInfo.face;
                ofresponse->requestInfo.customImgId = baseInfo.customImgId;

                reqeuestData.data = RSingleton<MsgWrap>::instance()->handleMsg(ofresponse);

                delete ofresponse;
                SendData(reqeuestData);

                iter++;
            }
        }

        //[2]查找该用户对应的聊天历史消息
        QList<TextRequest> textResponse;
        if(RSingleton<SQLProcess>::instance()->loadChatCache(db,request->accountId,textResponse))
        {
            QList<TextRequest>::iterator iter = textResponse.begin();
            while(iter != textResponse.end())
            {
                SocketOutData responseData;
                responseData.sockId = socketId;

                responseData.data = RSingleton<MsgWrap>::instance()->handleText(&(*iter));

                SendData(responseData);

                iter++;
            }
        }
    }

    delete request;
    delete response;
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

/*!
 * @brief 聊天消息处理
 * @param[in] db 数据库
 * @param[in] socketId 发送方SOCKET标识
 * @param[in] request 聊天信息主体
 * @return 是否插入成功
 */
void DataProcess::processText(Database *db, int socketId, TextRequest * request)
{
    SocketOutData responseData;

    if(request->type == SearchPerson)
    {
        TcpClient * client = TcpClientManager::instance()->getClient(request->otherSideId);
        if(client && ((OnlineStatus)client->getOnLineState() != STATUS_OFFLINE) )
        {
            responseData.sockId = client->socket();
            responseData.data = RSingleton<MsgWrap>::instance()->handleText(request);

            SendData(responseData);
        }
        else
        {
            if(request->type == SearchPerson)
            {
                //FIXME 存储消息时，会因存在'和"导致sql执行失败
                //TODO 扩充数据库表
                RSingleton<SQLProcess>::instance()->saveUserChat2Cache(db,request);
            }
            else if(request->type == SearchGroup)
            {

            }
        }
    }

    SocketOutData replyData;

    TextReply * reply = new TextReply;
    reply->applyType = APPLY_SYSTEM;
    reply->textId = request->textId;

    replyData.sockId = socketId;
    replyData.data = RSingleton<MsgWrap>::instance()->handleTextReply(reply);

    SendData(replyData);
    delete reply;

    delete request;
}
