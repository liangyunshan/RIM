/*!
 *  @brief     请求数据处理
 *  @details   处理解析后的请求数据，执行相应的数据库操作
 *  @file      dataprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180130:wey:添加分组操作
 *      20180131:wey:添加对文本信息处理；
 *      20180223:wey:添加用户登陆后，对离线系统消息抓取功能；
 *      20180224:wey:添加用户状态改变，用户状态推送功能；
 *      20180301:wey:添加删除用户好友；
 */
#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QSharedPointer>
#include <memory>

#include "protocoldata.h"
using namespace ProtocolType;

class Database;

class DataProcess
{
public:
    DataProcess();

    void processUserRegist(Database *db, int socketId,std::shared_ptr<RegistRequest>request);
    void processUserLogin(Database *db, int socketId, QSharedPointer<LoginRequest> request);
    void processUpdateUserInfo(Database * db, int socketId, QSharedPointer<UpdateBaseInfoRequest> request);
    void processUserStateChanged(Database *db, int socketId, QSharedPointer<UserStateRequest> request);

    void processSearchFriend(Database * db, int socketId, QSharedPointer<SearchFriendRequest> request);
    void processAddFriend(Database * db, int socketId, QSharedPointer<AddFriendRequest> request);
    void processRelationOperate(Database * db, int socketId, QSharedPointer<OperateFriendRequest> request);
    void processFriendList(Database *db, int socketId, QSharedPointer<FriendListRequest> request);
    void processGroupingOperate(Database *db, int socketId, QSharedPointer<GroupingRequest> request);
    void processGroupingFriend(Database *db, int socketId, QSharedPointer<GroupingFriendRequest> request);

    void processGroupList(Database *db, int socketId, QSharedPointer<ChatGroupListRequest> request);

    void processText(Database *db, int socketId, TextRequest *request);

    void processFileRequest(Database *db, int socketId, QSharedPointer<FileItemRequest> request);
    void processFileControl(Database *db, int socketId, QSharedPointer<SimpleFileItemRequest> request);
    void processFileData(Database *db, int socketId, QSharedPointer<FileDataRequest> request);

private:
    void pushFriendMyStatus(Database *db, int socketId, QString accountId, OnlineStatus onStatus);

};

#endif // DATAPROCESS_H
