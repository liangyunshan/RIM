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

    const QString accountId;                //系统个人账号基数，每次申请的账号均在此数字上加1。
    const QString groupAccoungId;           //系统群账号基数，每次正确申请后在此数字加1
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
    const QString systemIon;                //是否为系统头像，默认为系统头像true
    const QString iconId;                   //图片索引
};

/*!
 *  @brief   用户分组表
    @details 保存用户名下所有的分组信息 RUSER (1 - N) RGroup
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
    const QString index;                    //分组排序
};

/*!
 *  @brief 联系人分组描述表
 *  @details 描述用户分组的概览信息，方便查找联系人分组信息; @n
 *           1.创建用户时，同步创建记录; @n
 *           2.创建分组时，在当前groupids中插入对应group id，并以','分隔; @n
 *           3.移动分组时，调整groupids中对应group id的顺序; @n
 *           4.删除分组时，从groupids中移除对应的group id; @n
 */
class RGroupDesc
{
public:
    RGroupDesc();
    const QString table;
    const QString id;                       //user表id
    const QString account;                  //user表account
    const QString groupids;                 //用户拥有group id集合，以','分隔
    const QString groupsize;                //用户拥有group数量
};

/*!
 * @brief 分组-用户表
 * @details 保存用户分组中联系人的信息，通过RGroup中的id信息，找到该分组下所有的ID信息 @n
 *          RGROUP_USER (N - N) RGroup
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

/*!
 *  @brief 群表
 *  @details 描述一个群的基本信息，包括群名称，群主等
 */
class RChatRoom
{
public:
    RChatRoom();
    const QString table;
    const QString id;                       //UUID
    const QString chatId;                   //群号，可通过此号码搜索(如2xxxx0开始)
    const QString name;                     //群名称
    const QString desc;                     //群描述
    const QString label;                    //群标签
    const QString visible;                  //是否可见(是否允许被搜索到)
    const QString validate;                 //是否需加群验证 需新建表表示验证信息
    const QString question;                 //验证问题
    const QString answer;                   //验证答案
    const QString userId;                   //群所属用户ID即群主ID(同User中ID)
    const QString systemIon;                //是否为系统头像，默认为系统头像true
    const QString iconId;                   //图片索引
};

/*!
 *  @brief  群分组描述表
 *  @details 用户群分组表
 */
class RChatGroup
{
public:
    RChatGroup();
    const QString table;
    const QString id;                       //UUID
    const QString name;                     //群分组名
    const QString groupCount;               //群分组内群成员数量
    const QString userId;                   //群分组所属用户ID(同User中ID)
    const QString defaultGroup;             //默认分组(1表示默认分组，0表示非默认分组)
};

/*!
 *  @brief 群分组描述表
 *  @details 整合该用户所具备的群分组，主要便于排序显示
 */
class RChatGroupDesc
{
public:
    RChatGroupDesc();
    const QString table;
    const QString id;                       //user表id
    const QString account;                  //user表account
    const QString chatgroupids;             //用户拥有群group id集合，以','分隔
    const QString chatgroupsize;            //用户拥有group数量
};

/*!
 *  @brief 群分组RChatGroup-群表RChatRoom
 *  @details 明确了群分组下存在群的信息，即用户加入的群的描述信息
 */
class RChatGroupRoom
{
public:
    RChatGroupRoom();
    const QString table;
    const QString id;                       //UUID
    const QString chatroomId;               //群表UUID
    const QString chatgroupId;              //群分组表UUID
    const QString remarks;                  //群备注名
    const QString messNotifyLevel;          //消息通知等级 ChatMessNotifyLevel结构体(接收并提醒/接收不提醒/屏蔽群消息等)
};

/*!
 *  @brief 群成员表
 *  @details 保存该群中群成员基本信息
 */
class RChatroom_User
{
public:
    RChatroom_User();
    const QString table;
    const QString id;                       //UUID
    const QString chatroomId;               //对应群ID
    const QString userId;                   //成员ID(User表Id)
    const QString manager;                  //是否为管理员(0为普通用户;1为管理员)
    const QString remarks;                  //群备注(未设置则显示群名称，设置后显示此信息)
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

//用户聊天信息表
class RUserChatCache
{
public:
    RUserChatCache();
    const QString table;
    const QString id;
    const QString textId;                   /*!< 消息ID */
    const QString textType;                 /*!< 消息类型 */
    const QString encryption;               /*!< 是否加密 */
    const QString compress;                 /*!< 是否压缩 */
    const QString account;                  /*!< 用户ID(user Id) */
    const QString otherSideId;              /*!< 接收方用户ID */
    const QString data;                     /*!< 消息主体内容 */
    const QString time;                     /*!< 时间戳 */
    const QString msgType;                  /*!< 消息类型 */
};

/*!
 *  @brief 文件信息表
 */
class RFile
{
public:
    RFile();
    const QString table;
    const QString id;                       /*!< 消息ID */
    const QString md5;                      /*!< MD5 */
    const QString fileName;                 /*!< 文件名 */
    const QString quoteId;                  /*!< 当出现MD5存在时，将对应的数据ID，作为自己的引用，在数据读取时进行查询 */
    const QString quoteNum;                 /*!< 当前数据记录被引用的次数，默认为0，若不为0 ，则不可直接删除。 */
    const QString src;                      /*!< 发送用户 */
    const QString dst;                      /*!< 目的用户 */
    const QString dtime;                    /*!< 发送日期 */
    const QString fileSize;                 /*!< 文件大小 */
    const QString filePath;                 /*!< 文件存储的路径，后期可动态调整 */
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
