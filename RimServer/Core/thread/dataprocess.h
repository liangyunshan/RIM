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

#include <QScopedPointer>

#include "protocoldata.h"
using namespace ProtocolType;

class Database;

class DataProcess
{
public:
    DataProcess();

    void processUserRegist(Database *db, int socketId, RegistRequest *request);
    void processUserLogin(Database *db, int socketId, LoginRequest * request);
    void processUpdateUserInfo(Database * db,int socketId, UpdateBaseInfoRequest *request);
    void processUserStateChanged(Database *db, int socketId, UserStateRequest *request);

    void processSearchFriend(Database * db,int socketId, SearchFriendRequest *request);
    void processAddFriend(Database * db, int socketId, AddFriendRequest *request);
    void processRelationOperate(Database * db, int socketId, OperateFriendRequest *request);
    void processFriendList(Database *db, int socketId, FriendListRequest *request);
    void processGroupingOperate(Database *db, int socketId, GroupingRequest *request);
    void processGroupingFriend(Database *db, int socketId, GroupingFriendRequest *request);

    void processText(Database *db, int socketId, TextRequest *request);

private:
    void pushFriendMyStatus(Database *db, int socketId, QString accountId, OnlineStatus onStatus);

};

#endif // DATAPROCESS_H
