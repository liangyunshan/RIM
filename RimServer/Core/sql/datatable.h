/*!
 *  @brief     数据库表
 *  @details   分离硬编码，方便数据字段名称的调整。
 *  @file      datatable.h
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

//系统配置表，以key-value形式存储键值
class RimConfig
{
public:
    RimConfig();
    const QString table;
    const QString name;
    const QString value;

    const QString accuoutId;                //系统账号基数，每次申请的账号均在此数字上加1。
};

//用户信息表
class RUser
{
public:
    RUser();
    const QString table;
    const QString id;                       //UUID
    const QString account;                  //账号(登陆时使用)
    const QString password;                 //密码(加密存储)
    const QString nickName;                 //昵称
    const QString signName;                 //签名
    const QString gender;                   //性别
    const QString birthDay;                 //生日
    const QString phone;                    //联系方式
    const QString address;                  //地址
    const QString email;                    //邮箱
    const QString remark;                   //基本描述
    const QString face;                     //头像类型
    const QString faceId;                   //图片索引
};

/*!
  用户分组表
  保存用户名下所有的分组信息
  RUSE (1 - N) RGroup
*/
class RGroup
{
public:
    RGroup();
    const QString table;
    const QString id;                       //UUID
    const QString name;                     //分组名
    const QString userCount;                //分组成员数量
    const QString userId;                   //分组所属用户ID(同User中ID)
    const QString defaultGroup;             //默认分组(1表示默认分组，0表示非默认分组)
};

/*!
  分组-用户表
  保存用户分组中联系人的信息，通过RGroup中的id信息，找到该分组下所有的ID信息
  RGROUP_USER (N - 1) RGroup
*/
class RGroup_User
{
public:
    RGroup_User();
    const QString table;
    const QString id;                       //UUID
    const QString groupId;                  //分组ID(group id)
    const QString userId;                   //用户ID(user Id)
    const QString remarks;                  //备注
    const QString visible;                  //可见
};

//群信息表
class RChatRoom
{
public:
    RChatRoom();
    const QString table;
    const QString id;                       //UUID
    const QString name;                     //群名称
    const QString desc;                     //群描述
    const QString label;                    //群标签
    const QString userId;                   //群所属用户ID(同User中ID)
};

//用户-群信息表
class RChatroom_User
{
public:
    RChatroom_User();
    const QString id;                       //UUID
    const QString chatId;                   //对应群ID
    const QString userId;                   //群成员ID
};

//请求缓存表
class RequestCache
{
public:
    RequestCache();
    const QString table;
    const QString id;                       //UUID
    const QString account;                  //用户ID(user Id)
    const QString operateId;                //待操作用户ID(user id)
    const QString type;                     //请求类型(好友请求/同意请求/拒绝请求)
    const QString time;                     //时间戳
};

class RUserChatCache
{
public:
    RUserChatCache();
    const QString table;
    const QString id;                       //int
    const QString account;                  //用户ID(user Id)
    const QString destAccount;              //接收方用户ID
    const QString data;                     //消息主体内容
    const QString time;                     //时间戳
    const QString msgType;                  //消息类型
};

}

/*!

  @code
    SET FOREIGN_KEY_CHECKS=0;

    -- ----------------------------
    -- Table structure for `rchatroom`
    -- ----------------------------
    DROP TABLE IF EXISTS `rchatroom`;
    CREATE TABLE `rchatroom` (
      `ID` varchar(50) NOT NULL,
      `NAME` varchar(255) DEFAULT NULL,
      `DESC` varchar(255) DEFAULT NULL,
      `LABEL` varchar(255) DEFAULT NULL,
      `UID` varchar(50) NOT NULL,
      PRIMARY KEY (`ID`),
      KEY `UID` (`UID`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

    -- ----------------------------
    -- Records of rchatroom
    -- ----------------------------

    -- ----------------------------
    -- Table structure for `rchatroom_user`
    -- ----------------------------
    DROP TABLE IF EXISTS `rchatroom_user`;
    CREATE TABLE `rchatroom_user` (
      `ID` varchar(50) NOT NULL,
      `CID` varchar(50) NOT NULL,
      `UID` varchar(50) NOT NULL,
      PRIMARY KEY (`ID`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

    -- ----------------------------
    -- Records of rchatroom_user
    -- ----------------------------

    -- ----------------------------
    -- Table structure for `requestcache`
    -- ----------------------------
    DROP TABLE IF EXISTS `requestcache`;
    CREATE TABLE `requestcache` (
      `ID` varchar(50) NOT NULL,
      `ACCOUNT` varchar(11) NOT NULL,
      `OPERATEID` varchar(11) NOT NULL,
      `TYPE` int(4) NOT NULL,
      `TIME` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
      PRIMARY KEY (`ID`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

    -- ----------------------------
    -- Records of requestcache
    -- ----------------------------

    -- ----------------------------
    -- Table structure for `rgroup`
    -- ----------------------------
    DROP TABLE IF EXISTS `rgroup`;
    CREATE TABLE `rgroup` (
      `ID` varchar(50) NOT NULL,
      `NAME` varchar(50) NOT NULL,
      `USER_COUNT` int(4) DEFAULT NULL,
      `UID` varchar(50) NOT NULL,
      `DEFAUL` tinyint(2) NOT NULL,
      PRIMARY KEY (`ID`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

    -- ----------------------------
    -- Table structure for `rgroup_user`
    -- ----------------------------
    DROP TABLE IF EXISTS `rgroup_user`;
    CREATE TABLE `rgroup_user` (
      `ID` varchar(50) NOT NULL,
      `GID` varchar(50) NOT NULL,
      `UID` varchar(50) NOT NULL,
      `REMARKS` varchar(255) DEFAULT NULL,
      `VISIBLE` tinyint(4) DEFAULT NULL,
      PRIMARY KEY (`ID`),
      KEY `GID` (`GID`),
      KEY `UID` (`UID`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

    -- ----------------------------
    -- Table structure for `rimconfig`
    -- ----------------------------
    DROP TABLE IF EXISTS `rimconfig`;
    CREATE TABLE `rimconfig` (
      `NAME` varchar(50) NOT NULL,
      `VALUE` varchar(50) DEFAULT NULL
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

    -- ----------------------------
    -- Table structure for `ruser`
    -- ----------------------------
    DROP TABLE IF EXISTS `ruser`;
    CREATE TABLE `ruser` (
      `ID` varchar(50) NOT NULL,
      `ACCOUNT` varchar(11) NOT NULL,
      `PASSWORD` varchar(255) NOT NULL,
      `NICKNAME` varchar(255) NOT NULL,
      `SIGNNAME` varchar(255) DEFAULT NULL,
      `GENDER` tinyint(4) DEFAULT NULL,
      `BIRTHDAY` date DEFAULT NULL,
      `PHONE` varchar(29) DEFAULT NULL,
      `ADDRESS` varchar(255) DEFAULT NULL,
      `EMAIL` varchar(50) DEFAULT NULL,
      `REMARK` varchar(255) DEFAULT NULL,
      `FACE` tinyint(4) DEFAULT NULL,
      `FACEID` varchar(50) DEFAULT NULL,
      PRIMARY KEY (`ID`)

    -- ----------------------------
    -- Table structure for `ruserchatcache`
    -- ----------------------------
    DROP TABLE IF EXISTS `ruserchatcache`;
    CREATE TABLE `ruserchatcache` (
      `ID` int(10) NOT NULL AUTO_INCREMENT,
      `ACCOUNT` varchar(11) NOT NULL,
      `DESTACCOUNT` varchar(11) NOT NULL,
      `DATA` varchar(2000) NOT NULL,
      `TTSTAMP` bigint(20) DEFAULT NULL,
      `MSGTYPE` smallint(2) DEFAULT NULL,
      PRIMARY KEY (`ID`)
    ) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=utf8;
    @endcode

*/

#endif // TABLE_H
