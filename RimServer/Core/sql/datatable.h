/*!
 *  @brief     数据库表
 *  @details   分离硬编码，方便修改数据库
 *  @file      table.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATATABLE_H
#define DATATABLE_H

#include <QString>


namespace DataTable
{

//用户信息表
class User
{
public:
    User();
    const QString id;                       //UUID
    const QString account;                  //账号
    const QString name;                     //用户名
    const QString password;                 //密码(加密存储)
    const QString nickName;                 //昵称
    const QString signName;                 //签名
    const QString gender;                   //性别
    const QString birthDay;                 //生日
    const QString phone;                    //联系方式
    const QString address;                  //地址
    const QString email;                    //邮箱
    const QString desc;                     //基本描述
};

//用户分组表
class Group
{
public:
    Group();
    const QString id;                       //UUID
    const QString name;                     //分组名
    const QString userCount;                //分组成员数量
    const QString userId;                   //分组所属用户ID(同User中ID)
};

//用户-分组表
class User_Group
{
public:
    User_Group();
    const QString id;                       //UUID
    const QString groupId;                  //分组ID(group id)
    const QString userId;                   //用户ID(user Id)
    const QString remarks;                  //备注
    const QString visible;                  //可见
};

//群信息表
class ChatRoom
{
public:
    ChatRoom();
    const QString id;                       //UUID
    const QString name;                     //群名称
    const QString desc;                     //群描述
    const QString label;                    //群标签
    const QString userId;                   //群所属用户ID(同User中ID)
};

//用户-群信息表
class User_Chatroom
{
public:
    User_Chatroom();
    const QString id;                       //UUID
    const QString chatId;                   //对应群ID
    const QString userId;                   //群成员ID
};

}

#endif // TABLE_H
