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
class User
{
public:
    User();
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

//用户分组表
class Group
{
public:
    Group();
    const QString table;
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
    const QString table;
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

//请求缓存表
class RequestCache
{
public:
    RequestCache();
    const QString table;
    const QString id;                       //UUID
    const QString account;                  //用户ID(user Id)
    const QString requestId;                //发送请求用户ID(user id)
    const QString time;
};

}

/*!

Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50721
Source Host           : localhost:3306
Source Database       : rimserver

Target Server Type    : MYSQL
Target Server Version : 50721
File Encoding         : 65001

Date: 2018-01-16 15:34:57
SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `chatroom`
-- ----------------------------
DROP TABLE IF EXISTS `chatroom`;
CREATE TABLE `chatroom` (
  `ID` varchar(50) NOT NULL,
  `NAME` varchar(255) DEFAULT NULL,
  `DESC` varchar(255) DEFAULT NULL,
  `LABEL` varchar(255) DEFAULT NULL,
  `UID` varchar(50) NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `UID` (`UID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of chatroom
-- ----------------------------

-- ----------------------------
-- Table structure for `user`
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user` (
  `ID` varchar(50) NOT NULL,
  `ACCOUNT` int(11) NOT NULL,
  `NAME` varchar(50) NOT NULL,
  `PASSWORD` varchar(255) NOT NULL,
  `NICKNAME` varchar(255) DEFAULT NULL,
  `SIGNNAME` varchar(255) DEFAULT NULL,
  `GENDER` tinyint(4) DEFAULT NULL,
  `BIRTHDAY` date DEFAULT NULL,
  `PHONE` varchar(29) DEFAULT NULL,
  `ADDRESS` varchar(255) DEFAULT NULL,
  `EMAIL` varchar(50) DEFAULT NULL,
  `DESC` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of user
-- ----------------------------

-- ----------------------------
-- Table structure for `user_chatroom`
-- ----------------------------
DROP TABLE IF EXISTS `user_chatroom`;
CREATE TABLE `user_chatroom` (
  `ID` varchar(50) NOT NULL,
  `CID` varchar(50) NOT NULL,
  `UID` varchar(50) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of user_chatroom
-- ----------------------------

-- ----------------------------
-- Table structure for `user_group`
-- ----------------------------
DROP TABLE IF EXISTS `user_group`;
CREATE TABLE `user_group` (
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
-- Records of user_group
-- ----------------------------

*/

#endif // TABLE_H
