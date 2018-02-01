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
    void processSearchFriend(Database * db,int socketId, SearchFriendRequest *request);
    void processAddFriend(Database * db, int socketId, AddFriendRequest *request);
    void processRelationOperate(Database * db, int socketId, OperateFriendRequest *request);
    void processFriendList(Database *db, int socketId, FriendListRequest *request);
    void processGroupingOperate(Database *db, int socketId, GroupingRequest *request);

    void processText(Database *db, int socketId, TextRequest *request);
};

#endif // DATAPROCESS_H
