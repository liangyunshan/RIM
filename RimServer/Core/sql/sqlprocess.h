/*!
 *  @brief     数据执行处理
 *  @details   用于对数据库进行操作
 *  @file      sqlprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180201:wey:添加对用户分组好友的操作(创建、移动、修改)
 *      20180301:wey:修复查询默认分组时未加入默认分组条件bug
 *      20180418:wey:修复删除分组时，为将分组下联系人移动至默认分组;
 */
#ifndef SQLPROCESS_H
#define SQLPROCESS_H

#include "protocoldata.h"
using namespace ProtocolType;

#include "Network/tcpclient.h"

namespace Datastruct
{
struct FileItemInfo;
}

class Database;

class SQLProcess
{
public:
    SQLProcess();

    ResponseRegister processUserRegist(Database *db, const RegistRequest * request, QString & id, QString &uuid);
    ResponseLogin processUserLogin(Database *db, const LoginRequest * request);
    ResponseUpdateUser processUpdateUserInfo(Database *db, const UpdateBaseInfoRequest * request);
    ResponseAddFriend processSearchFriend(Database *db,SearchFriendRequest *request,SearchFriendResponse * response);
    ResponseAddFriend processAddFriendRequest(Database *db,QString accountId,QString operateId,int type);

    bool createGroup(Database *db, QString userId, QString groupName, QString groupId, bool isDefault = false);
    bool renameGroup(Database *db,GroupingRequest* request);
    bool deleteGroup(Database *db,GroupingRequest* request);
    bool testTstablishRelation(Database *db,OperateFriendRequest *request);
    bool establishRelation(Database *db,OperateFriendRequest *request);
    bool getFriendList(Database *db,QString accountId,FriendListResponse * response);
    void getFriendAccountList(Database *db, const QString accountId, QList<QString> &friendList);
    bool getUserInfo(Database *db, const QString accountId, UserBaseInfo &userInfo);

    bool updateGroupFriendInfo(Database *db,GroupingFriendRequest * request);
    bool updateMoveGroupFriend(Database *db,GroupingFriendRequest * request);
    bool deleteFriend(Database *db, GroupingFriendRequest * request, QString &accountId, QString &otherUserGroupId);

    bool loadSystemCache(Database * db,QString accountId,QList<AddFriendRequest> & requests);
    bool loadChatCache(Database * db, QString accountId, QList<TextRequest> &textResponse);

    bool saveUserChat2Cache(Database * db, TextRequest * request);

    bool addQuoteFile(Database * db,const FileItemRequest * request);
    bool queryFile(Database * db,const QString & fileMd5);
    bool addFile(Database * db, ServerNetwork::FileRecvDesc * desc);
    bool getFileInfo(Database * db, SimpleFileItemRequest *request, FileItemRequest * response);
    bool getDereferenceFileInfo(Database * db, SimpleFileItemRequest *request, Datastruct::FileItemInfo *itemInfo);

    QString getDefaultGroupByUserId(Database * db,const QString uuid);
    QString getDefaultGroupByUserAccountId(Database * db,const QString id);
    QStringList getGroupListByUserAccountId(Database * db,const QString id);

private:
    QStringList getGroupsById(Database * db,const QString id);
};

#endif // SQLPROCESS_H
