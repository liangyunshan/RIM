#include "dataprocess.h"

#include <QDebug>
#include <QThread>
#include <QScopedPointer>

#include "Util/rutil.h"
#include "rsingleton.h"
#include "sql/sqlprocess.h"
#include "Network/msgwrap/textwrapfactory.h"
#include "Network/msgwrap/binarywrapfactory.h"
#include "Network/head.h"
#include "Network/netglobal.h"
#include "global.h"
#include "protocol/datastruct.h"

#include "Network/tcpclient.h"
using namespace ServerNetwork;

#define FILE_MAX_PACK_SIZE 900      //文件每包最大的长度

#define SendData(data) { std::unique_lock<std::mutex> ul(G_SendMutex);\
                         G_SendButts.push(data);\
                         G_SendCondition.notify_one();}

DataProcess::DataProcess()
{

}

/*!
 * @brief 提供用户注册，返回注册的ID
 * @details  注册成功后：
 *          1.自动为用户创建一个默认的分组，并且名称命名为【我的好友】
 *          2.自动为用户创建一个默认群分组，并且命名为【我的群】
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void DataProcess::processUserRegist(Database *db, int socketId, std::shared_ptr<RegistRequest>request)
{
    SocketOutData data;
    data.sockId = socketId;

    QString registId,uuid;

    ResponseRegister regResult = RSingleton<SQLProcess>::instance()->processUserRegist(db,request.get(),registId,uuid);

    if(regResult == REGISTER_SUCCESS)
    {
        QScopedPointer<RegistResponse> response(new RegistResponse);
        response->accountId = registId;
        data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
    }
    else
    {
        data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,regResult);
    }

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
void DataProcess::processUserLogin(Database * db,int socketId, QSharedPointer<LoginRequest> request)
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
        loginResult = RSingleton<SQLProcess>::instance()->processUserLogin(db,request.data());
    }

    if(loginResult == LOGIN_SUCCESS)
    {
        QScopedPointer<LoginResponse> response(new LoginResponse);
        response->loginType = request->loginType;
        RSingleton<SQLProcess>::instance()->getUserInfo(db,request->accountId,response->baseInfo);
        data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());

        TcpClient * client = TcpClientManager::instance()->getClient(socketId);
        if(client)
        {
            client->setAccount(request->accountId);
            client->setNickName(response->baseInfo.nickName);
            client->setOnLineState((int)request->status);
        }
    }
    else
    {
        data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,loginResult);
    }

    SendData(data);

    //[1]向用户对应的好友推送上线通知。
    if(loginResult == LOGIN_SUCCESS){
        pushFriendMyStatus(db,socketId,request->accountId,request->status);
    }
}

void DataProcess::processUpdateUserInfo(Database * db,int socketId, QSharedPointer<UpdateBaseInfoRequest> request)
{
    SocketOutData data;
    data.sockId = socketId;

    ResponseUpdateUser updateResult = RSingleton<SQLProcess>::instance()->processUpdateUserInfo(db,request.data());

    if(updateResult == UPDATE_USER_SUCCESS)
    {
        QScopedPointer<UpdateBaseInfoResponse> response (new UpdateBaseInfoResponse);

        RSingleton<SQLProcess>::instance()->getUserInfo(db,request->baseInfo.accountId,response->baseInfo);
        data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
    }
    else
    {
        data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,updateResult);
    }

    SendData(data);
}

void DataProcess::processUserStateChanged(Database *db, int socketId, QSharedPointer<UserStateRequest> request)
{
    SocketOutData data;
    data.sockId = socketId;

    TcpClient * client = TcpClientManager::instance()->getClient(request->accountId);
    if(client)
    {
        client->setOnLineState((int)request->onStatus);

        QScopedPointer<UserStateResponse> response(new UserStateResponse);
        response->accountId = request->accountId;
        response->onStatus = request->onStatus;

        data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
    }
    else
    {
        data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,STATUS_FAILE);
    }

    SendData(data);

    pushFriendMyStatus(db,socketId,request->accountId,request->onStatus);
}

void DataProcess::processHistoryMsg(Database *db, int socketId, QSharedPointer<HistoryMessRequest> request)
{
    //[1]查找该用户对应的系统消息
    QList<AddFriendRequest> systemRequest;
    if(RSingleton<SQLProcess>::instance()->loadSystemCache(db,request->accountId,systemRequest))
    {
        QList<AddFriendRequest>::iterator iter = systemRequest.begin();
        while(iter != systemRequest.end()){
            SocketOutData reqeuestData;
            reqeuestData.sockId = socketId;

            QScopedPointer<OperateFriendResponse> ofresponse(new OperateFriendResponse());
            ofresponse->type = FRIEND_APPLY;
            ofresponse->result = (int)FRIEND_REQUEST;
            ofresponse->stype = (*iter).stype;
            ofresponse->accountId = request->accountId;

            UserBaseInfo baseInfo;
            RSingleton<SQLProcess>::instance()->getUserInfo(db,(*iter).accountId,baseInfo);
            ofresponse->requestInfo.accountId = baseInfo.accountId;
            ofresponse->requestInfo.nickName = baseInfo.nickName;
            ofresponse->requestInfo.signName = baseInfo.signName;
            ofresponse->requestInfo.isSystemIcon = baseInfo.isSystemIcon;
            ofresponse->requestInfo.iconId = baseInfo.iconId;

            reqeuestData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(ofresponse.data());

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

            responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(&(*iter));

            SendData(responseData);

            iter++;
        }
    }
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
    Q_UNUSED(socketId);
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

            QScopedPointer<UserStateResponse> response (new UserStateResponse);
            response->accountId = accountId;
            response->onStatus = onStatus;

            tmpData.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
            SendData(tmpData);
        }
        iter++;
    }
}

void DataProcess::processSearchFriend(Database * db,int socketId, QSharedPointer<SearchFriendRequest> request)
{
    SocketOutData data;
    data.sockId = socketId;

    if(request->stype == OperatePerson){
        QScopedPointer<SearchFriendResponse> response (new SearchFriendResponse);
        ResponseAddFriend updateResult = RSingleton<SQLProcess>::instance()->processSearchFriend(db,request.data(),response.data());

        if(updateResult == FIND_FRIEND_FOUND)
            data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
        else
            data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,updateResult);
    }else if(request->stype == OperateGroup){
        QScopedPointer<SearchGroupResponse> response (new SearchGroupResponse);
        ResponseAddFriend updateResult = RSingleton<SQLProcess>::instance()->processSearchGroup(db,request.data(),response.data());

        if(updateResult == FIND_FRIEND_FOUND)
            data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data(),FIND_FRIEND_FOUND);
        else
            data.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,MSG_GROUP_SEARCH,updateResult);
    }

    SendData(data);
}

/*!
 * @brief 处理用户响应好友请求操作(添加联系人或群)
 * @details A请求B，B向A回复结果。
 *          根据B的回复结果，若同意请求，则将对方的信息发送给对方，即发送A至B，发送B至A；
 *          若B拒绝请求，则直接将结果发送至A
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void DataProcess::processAddFriend(Database * db,int socketId, QSharedPointer<AddFriendRequest> request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    bool error = false;
    ResponseAddFriend result = ADD_FRIEND_SENDED;

    QString chatroomId,chatroomName;

    QString operateUserAccountId;
    if(request->stype == OperateGroup){
        if(!RSingleton<SQLProcess>::instance()->getUserByChatroomId(db,request->operateId,operateUserAccountId)){
            result = ADD_FRIEND_SENDED_FAILED;
            error = true;
        }else{
            ChatBaseInfo baseInfo;
            if(RSingleton<SQLProcess>::instance()->getChatroomInfo(db,request->operateId,baseInfo)){
                chatroomId = baseInfo.chatId;
                chatroomName = baseInfo.name;
            }
        }
    }else{
        operateUserAccountId = request->operateId;
    }

    if(!error){
        TcpClient * client = TcpClientManager::instance()->getClient(operateUserAccountId);
        if(client && ((OnlineStatus)client->getOnLineState() != STATUS_OFFLINE) )
        {
            SocketOutData reqeuestData;
            reqeuestData.sockId = client->socket();

            QScopedPointer<OperateFriendResponse> ofresponse (new OperateFriendResponse());
            ofresponse->type = FRIEND_APPLY;
            ofresponse->result = (int)FRIEND_REQUEST;
            ofresponse->stype = request->stype;
            ofresponse->accountId = client->getAccount();
            ofresponse->chatId = chatroomId;
            ofresponse->chatName = chatroomName;

            UserBaseInfo baseInfo;
            RSingleton<SQLProcess>::instance()->getUserInfo(db,request->accountId,baseInfo);
            ofresponse->requestInfo.accountId = baseInfo.accountId;
            ofresponse->requestInfo.nickName = baseInfo.nickName;
            ofresponse->requestInfo.signName = baseInfo.signName;
            ofresponse->requestInfo.isSystemIcon = baseInfo.isSystemIcon;
            ofresponse->requestInfo.iconId = baseInfo.iconId;

            reqeuestData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(ofresponse.data());

            SendData(reqeuestData);
        }else{
            result = RSingleton<SQLProcess>::instance()->processAddFriendRequest(db,request->accountId,request->operateId,(int)FRIEND_REQUEST);
        }
    }

    responseData.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,result);

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
void DataProcess::processRelationOperate(Database *db, int socketId, QSharedPointer<OperateFriendRequest> request)
{
    bool flag = false;

    ResponseFriendApply result = (ResponseFriendApply)request->result;
    if(result == FRIEND_AGREE){
        if(RSingleton<SQLProcess>::instance()->testEstablishRelation(db,request.data()))
            return;
        flag = RSingleton<SQLProcess>::instance()->establishRelation(db,request.data());
    }
    if(request->stype == OperatePerson){
        //【1】向自己发送对方联系人基本信息
        if(flag)
        {
            SocketOutData responseData;
            responseData.sockId = socketId;

            QScopedPointer<GroupingFriendResponse> responseA (new GroupingFriendResponse);
            responseA->type = G_Friend_CREATE;
            responseA->stype = request->stype;
            responseA->groupId = RSingleton<SQLProcess>::instance()->getDefaultGroupByUserAccountId(db,OperatePerson,request->accountId);

            UserBaseInfo baseInfo;
            RSingleton<SQLProcess>::instance()->getUserInfo(db,request->operateId,baseInfo);
            responseA->user.accountId = baseInfo.accountId;
            responseA->user.nickName = baseInfo.nickName;
            responseA->user.signName = baseInfo.signName;
            responseA->user.isSystemIcon = baseInfo.isSystemIcon;
            responseA->user.iconId = baseInfo.iconId;
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

            responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(responseA.data());
            SendData(responseData);
        }

        TcpClient * client = TcpClientManager::instance()->getClient(request->operateId);

        if(client && ((OnlineStatus)client->getOnLineState() != STATUS_OFFLINE) )
        {
            //【2】向对方发送此次好友请求的处理结果信息
            int operateSock = client->socket();
            SocketOutData reqeuestData;
            reqeuestData.sockId = operateSock;

            QScopedPointer<OperateFriendResponse> ofresponse(new OperateFriendResponse());
            ofresponse->type = FRIEND_APPLY;
            ofresponse->result = (int)request->result;
            ofresponse->stype = request->stype;
            ofresponse->accountId = client->getAccount();

            UserBaseInfo baseInfo;
            RSingleton<SQLProcess>::instance()->getUserInfo(db,request->accountId,baseInfo);
            ofresponse->requestInfo.accountId = baseInfo.accountId;
            ofresponse->requestInfo.nickName = baseInfo.nickName;
            ofresponse->requestInfo.signName = baseInfo.signName;
            ofresponse->requestInfo.isSystemIcon = baseInfo.isSystemIcon;
            ofresponse->requestInfo.iconId = baseInfo.iconId;

            reqeuestData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(ofresponse.data());

            SendData(reqeuestData);

            //【3】若同意请求，则再次向对方发送自己的基本信息
            if(result == FRIEND_AGREE && flag)
            {
                SocketOutData responseData;
                responseData.sockId = operateSock;

                QScopedPointer<GroupingFriendResponse> responseB(new GroupingFriendResponse());
                responseB->type = G_Friend_CREATE;
                responseB->stype = request->stype;

                responseB->groupId = RSingleton<SQLProcess>::instance()->getDefaultGroupByUserAccountId(db,OperatePerson,request->operateId);

                responseB->user.accountId = baseInfo.accountId;
                responseB->user.nickName = baseInfo.nickName;
                responseB->user.signName = baseInfo.signName;
                responseB->user.isSystemIcon = baseInfo.isSystemIcon;
                responseB->user.iconId = baseInfo.iconId;
                responseB->user.remarks = baseInfo.nickName;

                TcpClient * operateClient = TcpClientManager::instance()->getClient(request->accountId);
                if(operateClient)
                    responseB->user.status = (OnlineStatus)operateClient->getOnLineState();
                else
                    responseB->user.status = STATUS_OFFLINE;

                responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(responseB.data());
                SendData(responseData);
            }
        }else{
            RSingleton<SQLProcess>::instance()->processAddFriendRequest(db,request->accountId,request->operateId,(int)request->result);
        }
    }else if(request->stype == OperateGroup){
        //【1】向自己发送对方联系人基本信息
        UserBaseInfo baseInfo;
        if(flag)
        {
            SocketOutData responseData;
            responseData.sockId = socketId;

            QScopedPointer<GroupingFriendResponse> responseA (new GroupingFriendResponse);
            responseA->type = G_Friend_CREATE;
            responseA->stype = request->stype;
            responseA->groupId = RSingleton<SQLProcess>::instance()->getDefaultGroupByUserAccountId(db,OperateGroup,request->accountId);

            RSingleton<SQLProcess>::instance()->getUserInfo(db,request->operateId,baseInfo);
            responseA->user.accountId = baseInfo.accountId;
            responseA->user.nickName = baseInfo.nickName;
            responseA->user.signName = baseInfo.signName;
            responseA->user.isSystemIcon = baseInfo.isSystemIcon;
            responseA->user.iconId = baseInfo.iconId;
            responseA->user.remarks = baseInfo.nickName;

            TcpClient * operateClient = TcpClientManager::instance()->getClient(request->operateId);
            if(operateClient)
                responseA->user.status = (OnlineStatus)operateClient->getOnLineState();
            else
                responseA->user.status = STATUS_OFFLINE;

            responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(responseA.data());
            SendData(responseData);
        }

        TcpClient * client = TcpClientManager::instance()->getClient(request->operateId);

        if(client && ((OnlineStatus)client->getOnLineState() != STATUS_OFFLINE) )
        {
            //【2】向对方发送此次好友请求的处理结果信息
            int operateSock = client->socket();
            SocketOutData reqeuestData;
            reqeuestData.sockId = operateSock;

            QScopedPointer<OperateFriendResponse> ofresponse(new OperateFriendResponse());
            ofresponse->type = FRIEND_APPLY;
            ofresponse->result = (int)request->result;
            ofresponse->stype = request->stype;
            ofresponse->accountId = client->getAccount();
            ofresponse->chatId = request->chatId;
            ofresponse->chatName = request->chatName;

            reqeuestData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(ofresponse.data());

            SendData(reqeuestData);

            //【3】若同意请求，则再次向对方发送群的基本信息
            if(result == FRIEND_AGREE && flag)
            {
                SocketOutData responseData;
                responseData.sockId = operateSock;

                QScopedPointer<GroupingChatResponse> responseB(new GroupingChatResponse());

                responseB->type = G_ChatGroup_CREATE;
                responseB->groupId = RSingleton<SQLProcess>::instance()->getDefaultGroupByUserAccountId(db,OperateGroup,request->operateId);

                if(!RSingleton<SQLProcess>::instance()->getSimpleChatInfoByChatroomId(db,responseB->groupId,responseB->chatInfo)){

                }

                responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(responseB.data());
                SendData(responseData);
            }

            //todo 20180427 【4】向群内在线的用户推送成员添加信息

        }else{
            RSingleton<SQLProcess>::instance()->processAddFriendRequest(db,request->accountId,request->operateId,(int)request->result);
        }
    }
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
void DataProcess::processFriendList(Database *db, int socketId, QSharedPointer<FriendListRequest> request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    QScopedPointer<FriendListResponse> response (new FriendListResponse);
    response->type = request->type;
    response->accountId = request->accountId;
    bool flag = RSingleton<SQLProcess>::instance()->getFriendList(db,request->accountId,response.data());
    if(flag){
        for(int i = 0; i < response->groups.size(); i++){
            QList<SimpleUserInfo *>::iterator iter = response->groups.at(i)->users.begin();
            while(iter != response->groups.at(i)->users.end()){
                TcpClient * client = TcpClientManager::instance()->getClient((*iter)->accountId);
                if(client){
                    (*iter)->status = (OnlineStatus)client->getOnLineState();
                }else{
                    (*iter)->status = STATUS_OFFLINE;
                }
                iter++;
            }
        }
        responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
    }
    else
    {
        responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,(int)STATUS_FAILE);
    }

    SendData(responseData);
}

void DataProcess::processGroupingOperate(Database *db, int socketId, QSharedPointer<GroupingRequest> request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    bool flag = false;

    switch(request->type)
    {
        case GROUPING_CREATE:
            {
               flag = RSingleton<SQLProcess>::instance()->createGroupAndGroupDesc(db,request->gtype,request->uuid,"",request->groupId,request->groupName);
            }
            break;
        case GROUPING_RENAME:
            {
               flag = RSingleton<SQLProcess>::instance()->renameGroup(db,request.data());
            }
            break;
        case GROUPING_DELETE:
            {
                flag = RSingleton<SQLProcess>::instance()->deleteGroup(db,request.data());
            }
            break;
        case GROUPING_SORT:
            {
                 flag = RSingleton<SQLProcess>::instance()->sortGroupInGroupDesc(db,request.data());
            }
            break;
        default:
            break;
    }

    QScopedPointer<GroupingResponse> response (new GroupingResponse);
    response->uuid = request->uuid;
    response->gtype = request->gtype;
    response->type = request->type;
    response->groupId = request->groupId;
    response->groupIndex = request->groupIndex;
    responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data(),flag?STATUS_SUCCESS:STATUS_FAILE);

    SendData(responseData);
}

/*!
 * @brief 用户分组操作
 * @details 用于处理用户的信息更新、好友移动、好友删除操作
 * @param[in] db 数据库
 * @param[in] socketId 网络标识
 * @param[in] request 数据请求
 * @return 无
 */
void DataProcess::processGroupingFriend(Database *db, int socketId, QSharedPointer<GroupingFriendRequest> request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    bool flag = false;
    QString otherUserGroupId;
    QString accountId;

    switch(request->type)
    {
        case G_Friend_UPDATE:
            {
                flag = RSingleton<SQLProcess>::instance()->updateGroupFriendInfo(db,request.data());
            }
            break;
        case G_Friend_MOVE:
            {
                flag = RSingleton<SQLProcess>::instance()->updateMoveGroupFriend(db,request.data());
            }
            break;
        case G_Friend_Delete:
            {
                flag = RSingleton<SQLProcess>::instance()->deleteFriend(db,request.data(),accountId,otherUserGroupId);
            }
            break;
        default:
            break;
    }

    QScopedPointer<GroupingFriendResponse> response (new GroupingFriendResponse());
    response->type = request->type;
    response->stype = request->stype;
    response->groupId = request->groupId;
    response->oldGroupId = request->oldGroupId;
    response->user = request->user;

    if(flag)
        responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data(),STATUS_SUCCESS);
    else
        responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data(),STATUS_FAILE);
    SendData(responseData);

    //删除时，若对方在线，向对方推送消息
    if(flag && request->type == G_Friend_Delete)
    {
        TcpClient * client = TcpClientManager::instance()->getClient(request->user.accountId);
        if(client)
        {
            SocketOutData pushData;
            pushData.sockId = client->socket();

            QScopedPointer<GroupingFriendResponse> pushResponse (new GroupingFriendResponse());
            pushResponse->type = request->type;
            pushResponse->stype = request->stype;
            pushResponse->groupId = otherUserGroupId;
            pushResponse->oldGroupId = otherUserGroupId;

            //客户端只需要对方ID来查找对应的ToolItem
            pushResponse->user.accountId = accountId;

            pushData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(pushResponse.data(),STATUS_SUCCESS);
            SendData(pushData);
        }
    }
}

/*!
 * @brief 请求群分组列表
 * @details 登陆成功或者刷新列表时，请求该账户下的群分组信息
 * @param[in] db 数据库
 * @param[in] socketId 网络标识
 * @param[in] request 分组列表请求
 */
void DataProcess::processGroupList(Database *db, int socketId, QSharedPointer<ChatGroupListRequest> request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    QScopedPointer<ChatGroupListResponse> response (new ChatGroupListResponse);
    response->uuid = request->uuid;
    bool flag = RSingleton<SQLProcess>::instance()->getGroupList(db,request->uuid,response.data());
    if(flag){
        responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
    }else{
        responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,(int)STATUS_FAILE);
    }

    SendData(responseData);
}

/*!
 * @brief 群账户注册
 * @details 1.创建一个群，并将此用户作为群主；
 *          2.将群索引插入至当前用户默认群分组列表中;
 */
void DataProcess::processRegistGroup(Database *db, int socketId, QSharedPointer<RegistGroupRequest> request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    QScopedPointer<RegistGroupResponse> response (new RegistGroupResponse);
    ResponseRegister regResult = RSingleton<SQLProcess>::instance()->registGroup(db,request.data(),response.data());
    if(regResult == REGISTER_SUCCESS){
        //[1]返回注册成功消息
        if(RSingleton<SQLProcess>::instance()->getSingleChatGroupInfo(db,response.data()))
            responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());

        //[2]返回创建的群信息
        SocketOutData createChatResponse;
        createChatResponse.sockId = socketId;

        QScopedPointer<GroupingChatResponse> responseB(new GroupingChatResponse());

        responseB->type = G_ChatGroup_CREATE;
        responseB->groupId = RSingleton<SQLProcess>::instance()->getDefaultGroupByUserAccountId(db,OperateGroup,request->accountId);

        if(!RSingleton<SQLProcess>::instance()->getSimpleChatInfoByChatroomId(db,responseB->groupId,responseB->chatInfo)){

        }

        createChatResponse.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(responseB.data());
        SendData(createChatResponse);
    }else{
        responseData.data =  RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,regResult);
    }

    SendData(responseData);
}

/*!
 * @brief 请求执行群相关操作
 * @details 1.退出群：
 *          2.移出群：
 *          3.解散群：
 * @param[in] db 数据库
 * @param[in] socketId 发送方SOCKET标识
 * @param[in] request 请求主体
 */
void DataProcess::processGroupCommand(Database *db, int socketId, QSharedPointer<GroupingCommandRequest> request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    bool flag = false;
    switch(request->type){
        case G_ChatGroup_EXIT:
            {
                flag = RSingleton<SQLProcess>::instance()->exitGroupChat(db,request.data());
            }
            break;
        case G_ChatGroup_KICK:
            {

            }
            break;
        case G_ChatGroup_DISSOLVED:
            break;
        default:
            break;
    }

    if(flag){
        //[1]向客户端回复结果
        QScopedPointer<GroupingCommandResponse> response(new GroupingCommandResponse());
        response->respType = MSG_REPLY;
        response->type = request->type;
        response->accountId = request->accountId;
        response->groupId = request->groupId;
        response->chatRoomId = request->chatRoomId;
        response->operateId = request->operateId;

        responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
        //TODO [2]向其它客户端通知对应消息
    }else{
        responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsgReply(request->msgType,request->msgCommand,STATUS_FAILE,(int)request->type);
    }

    SendData(responseData);
}

/*!
 * @brief 聊天消息处理
 * @param[in] db 数据库
 * @param[in] socketId 发送方SOCKET标识
 * @param[in] request 聊天信息主体
 */
void DataProcess::processText(Database *db, int socketId, QSharedPointer<TextRequest> request)
{
    SocketOutData responseData;

    if(request->type == OperatePerson)
    {
        TcpClient * client = TcpClientManager::instance()->getClient(request->otherSideId);
        if(client && ((OnlineStatus)client->getOnLineState() != STATUS_OFFLINE) )
        {
            responseData.sockId = client->socket();
            responseData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(request.data());

            SendData(responseData);
        }else{
            if(request->type == OperatePerson)
            {
                //TODO 扩充数据库表
                RSingleton<SQLProcess>::instance()->saveUserChat2Cache(db,request);
            }
            else if(request->type == OperateGroup)
            {

            }
        }
    }

    SocketOutData replyData;

    QScopedPointer<TextReply> reply (new TextReply);
    reply->applyType = APPLY_SYSTEM;
    reply->textId = request->textId;

    replyData.sockId = socketId;
    replyData.data = RSingleton<TextWrapFactory>::instance()->getTextWrap()->handleMsg(reply.data());

    SendData(replyData);
}

/*!
 * @brief 处理文件发送请求或处理发送文件
 * @param[in] db 数据库连接
 * @param[in] socketId 通信ID
 * @param[in] request 文件传输请求
 */
void DataProcess::processFileRequest(Database *db, int socketId, QSharedPointer<FileItemRequest> request)
{
    SocketOutData responseData;
    responseData.sockId = socketId;

    if(request->itemType == FILE_ITEM_CHAT_UP || request->itemType == FILE_ITEM_USER_UP)
    {
        TcpClient * tmpClient = TcpClientManager::instance()->getClient(socketId);

        FileRecvDesc * desc = new FileRecvDesc;
        desc->itemType = (int)request->itemType;
        desc->itemKind = (int)request->itemKind;
        desc->size = request->size;
        desc->fileName = request->fileName;
        desc->md5 = request->md5;
        desc->fileId = request->fileId;
        desc->accountId = request->accountId;
        desc->otherId = request->otherId;
        desc->writeLen = 0;

        //查询是否已经存在此md5文件，若由的话直接在数据库中建立索引关系，避免再次上传
        bool exist = RSingleton<SQLProcess>::instance()->queryFile(db,request->md5);

        if(exist)
        {
            bool quoteResult = RSingleton<SQLProcess>::instance()->addQuoteFile(db,request.data());
            if(quoteResult)
            {
                QScopedPointer<SimpleFileItemRequest> response (new SimpleFileItemRequest);
                response->control = T_SERVER_EXIST;
                response->itemType = request->itemType;
                response->itemKind = request->itemKind;
                response->fileId = request->fileId;
                response->md5 = request->md5;

                responseData.data = RSingleton<BinaryWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
            }
            else
            {
                //TODO 引用失败后，因提示用户
            }
        }
        else
        {
            if(tmpClient && tmpClient->addFile(request->fileId,desc))
            {
                tmpClient->setAccount(request->accountId);
                QScopedPointer<SimpleFileItemRequest> response (new SimpleFileItemRequest);
                response->control = T_ABLE_SEND;
                response->itemType = request->itemType;
                response->itemKind = request->itemKind;
                response->fileId = request->fileId;
                response->md5 = request->md5;

                responseData.data = RSingleton<BinaryWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
            }
        }
    }

    SendData(responseData);
}

/*!
 * @details A:响应用户文件请求、文件状态控制，需要注意的是，在接收到用户控制报文时有以下情况： @n
 *              1.控制报文比其它数据报文先行处理，此时文件可能未被创建 @n
 *              2.控制报文在数据报文中处理，在设置完文件状态后，不能直接丢弃之后的数据，也需要缓存起来 @n
 *              3.控制报文在最后处理，此时所有的数据报文均已经处理完成，那么此时控制报文已经没有意义 @n
 *          B:文件下载请求: @n
 *              1.查找对应文件的基本信息反馈客户端 @n
 */
void DataProcess::processFileControl(Database *db, int socketId, QSharedPointer<SimpleFileItemRequest> request)
{
    //上传文件控制报文
    if(request->itemType == FILE_ITEM_CHAT_UP || request->itemType == FILE_ITEM_USER_UP)
    {
        TcpClient * tmpClient = TcpClientManager::instance()->getClient(socketId);
        if(tmpClient)
        {
            FileRecvDesc * desc = tmpClient->getFile(request->fileId);
            if(desc)
            {
                desc->lock();
                if(request->control == T_PAUSE)
                {
                    desc->setState(FILE_PAUSE);
                }
                else if(request->control == T_TRANING)
                {
                    desc->setState(FILE_TRANING);
                }
                else if(request->control == T_CANCEL)
                {
                    desc->setState(FILE_CANCEL);
                    desc->destory();
                }

                SocketOutData responseData;
                responseData.sockId = socketId;

                SimpleFileItemRequest * response = new SimpleFileItemRequest;
                response->control = request->control;
                response->itemKind = request->itemKind;
                response->itemType = request->itemType;
                response->md5 = request->md5;
                responseData.data = RSingleton<BinaryWrapFactory>::instance()->getTextWrap()->handleMsg(response);
                SendData(responseData);

                desc->unlock();
            }
        }
    }
    //下载文件请求报文
    else if(request->itemType == FILE_ITEM_CHAT_DOWN || request->itemType == FILE_ITEM_USER_DOWN)
    {
        if(request->control == T_REQUEST)
        {
            QScopedPointer<FileItemRequest> response (new FileItemRequest);
            response->itemType = request->itemType;
            bool flag = RSingleton<SQLProcess>::instance()->getFileInfo(db,request.data(),response.data());
            if(flag)
            {
                SocketOutData responseData;
                responseData.sockId = socketId;
                responseData.data = RSingleton<BinaryWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());
                SendData(responseData);
            }
        }
        else if(request->control == T_ABLE_SEND)
        {
            Datastruct::FileItemInfo fileItemInfo;
            bool flag = RSingleton<SQLProcess>::instance()->getDereferenceFileInfo(db,request.data(),&fileItemInfo);
            if(flag)
            {
                QFile file(fileItemInfo.filePath + "/" +fileItemInfo.md5);
                if(!file.open(QFile::ReadOnly))
                {
                    //TODO 提示客户端文件不存在
                    return;
                }

                int sendLen = 0;
                int currIndex = 0;
                while(!file.atEnd())
                {
                    SocketOutData responseData;
                    responseData.sockId = socketId;

                    FileDataTrunkRequest response;
                    response.md5 = request->fileId;
                    response.currIndex = currIndex++;
                    response.data = file.read(900);
                    sendLen += response.data.size();
                    responseData.data = RSingleton<BinaryWrapFactory>::instance()->getTextWrap()->handleMsg((MsgPacket *)&response);
                    SendData(responseData);
                }
            }
        }
    }
}

/*!
 *  @details 在传输过程中，因控制报文可在数据报文中间,则在文件处理时，采用以下策略: @n
 *           1.暂停传输/恢复传输：数据报文接收后继续往文件中写入，不丢弃； @n
 *           2.取消传输；将文件关闭并删除，之后的数据报文直接丢弃。
 */
void DataProcess::processFileData(Database *db, int socketId, QSharedPointer<FileDataRequest> request)
{
    TcpClient * tmpClient = TcpClientManager::instance()->getClient(socketId);
    if(tmpClient)
    {
        FileRecvDesc * desc = tmpClient->getFile(request->fileId);
        if(desc)
        {
            desc->lock();
            if(desc->state() == FILE_CANCEL)
            {
                desc->unlock();
                return;
            }

            if(desc->isNull() && !desc->create(RGlobal::G_FILE_UPLOAD_PATH))
            {
                desc->unlock();
                return;
            }

            if(desc->state() == FILE_TRANING || desc->state() == FILE_PAUSE)
                if(desc->seek(request->index * FILE_MAX_PACK_SIZE) && desc->write(request->array) > 0)
                {
                    if(desc->flush() && desc->isRecvOver())
                    {
                        desc->close();
                    }
                }

            desc->unlock();

            if(desc->isRecvOver())
            {
                if(RSingleton<SQLProcess>::instance()->addFile(db,desc))
                {
                       SocketOutData responseData;
                       responseData.sockId = socketId;

                       QScopedPointer<SimpleFileItemRequest> response (new SimpleFileItemRequest);
                       response->control = T_OVER;
                       response->itemType = static_cast<FileItemType>(desc->itemType);
                       response->itemKind = static_cast<FileItemKind>(desc->itemKind);
                       response->md5 = desc->md5;
                       response->fileId = desc->fileId;
                       responseData.data = RSingleton<BinaryWrapFactory>::instance()->getTextWrap()->handleMsg(response.data());

                       SendData(responseData);
                }
                tmpClient->removeFile(request->fileId);
            }
        }
    }
}
