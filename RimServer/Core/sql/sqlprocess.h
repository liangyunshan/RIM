/*!
 *  @brief     数据执行处理
 *  @details   用于对数据库进行操作
 *  @file      sqlprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SQLPROCESS_H
#define SQLPROCESS_H


#include "protocoldata.h"
using namespace ProtocolType;

class Database;

class SQLProcess
{
public:
    SQLProcess();

    ResponseRegister processUserRegist(Database *db, const RegistRequest * request,QString & id);
    ResponseLogin processUserLogin(Database *db, const LoginRequest * request);
    ResponseUpdateUser processUpdateUserInfo(Database *db, const UpdateBaseInfoRequest * request);
    ResponseAddFriend processSearchFriend(Database *db,SearchFriendRequest *request,SearchFriendResponse * response);
    ResponseAddFriend processAddFriend(Database *db,AddFriendRequest *request);

    void getUserInfo(Database *db, const QString accountId, UserBaseInfo &userInfo);

private:

};

#endif // SQLPROCESS_H
