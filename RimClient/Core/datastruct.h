﻿/*!
 *  @brief     数据定义
 *  @details   定义了软件运行时所有的结构体、枚举等
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <QObject>
#include <QString>
#include <QDataStream>

#include "protocoldata.h"
using namespace ProtocolType;

template<class T>
class GlobalData
{
protected:
    GlobalData(){}
    virtual ~GlobalData(){}
    T * q_ptr;
};

/*!
 *  @brief  系统通知消息的类型
 *  @details 程序内部提供了系统级别的通知-订阅模式，此数据类型用于区分广播的类型。订阅者一句类型进行对应的处理。
 */
enum MessageType
{
    MESS_STYLE,                  /*!< 样式更新 */
    MESS_SHORTCUT,               /*!< 快捷键更新 */
    MESS_SETTINGS,               /*!< 系统设置修改 */
    MESS_TEXT_NET_ERROR,         /*!< 信息信息服务器错误 */
    MESS_TEXT_NET_OK,            /*!< 信息信息服务器正常 */
    MESS_FILE_NET_ERROR,         /*!< 文件信息服务器错误 */
    MESS_FILE_NET_OK,            /*!< 文件信息服务器正常 */
    MESS_BASEINFO_UPDATE,        /*!< 基本信息修改 */
    MESS_RELATION_FRIEND_ADD,    /*!< 分组中添加好友 */
    MESS_RELATION_GROUP_ADD,     /*!< 群分组中添加新群 */
    MESS_NOTIFY_WINDOWS,         /*!< 显示消息通知窗口 */
    MESS_ICON_CHANGE,            /*!< 登陆用户icon改变 */
    MESS_SCREEN_CHANGE,          /*!< 屏幕改变 */
    MESS_CHATGROUP_REGIST_SUCCESS,      /*!< 群账户注册成功 */
    MESS_CHATGROUP_REGIST_FAILED,       /*!< 群账户注册失败 */
    MESS_FRIEND_STATE_CHANGE,     /*!< 好友状态改变 */
    MESS_ADD_FRIEND_WINDOWS,      /*!< 打开添加好友 */
    MESS_USER_OFF_LINE            /*!< 用户下线 */
};

/*!
  @brief 登陆用户描述信息
  @details 用于登陆时从本地文件读取并保存登陆用户的基本信息，在登陆成功后，将登陆信息保存至本地配置文件。
*/
struct UserInfoDesc
{
    UserInfoDesc()
    {
        isRemberPassword = false;
        isAutoLogin = false;
        isSystemIcon = true;
    }
    QString userName;               /*!< 用户名 */
    QString accountId;              /*!< 账号*/
    int loginState;                 /*!< 登录状态，参见OnLineState::UserState枚举*/
    QString originPassWord;         /*!< 原始密码 */
    QString password;               /*!< 加密后的密码 */
    bool isRemberPassword;          /*!< 是否记住密码 */
    bool isAutoLogin;               /*!< 是否自动登录 */
    bool isSystemIcon;              /*!< 是否为系统图标，默认为true，修改为自定义图标后为false */
    QString iconId;                 /*!< 头像ID，isSystemIcon为true时，值为系统图标文件名【1.png】；为false时，默认在【账户ID/ChatRecvFile】文件夹下 */

    friend QDataStream & operator <<(QDataStream & stream,const UserInfoDesc & desc);
    friend QDataStream & operator >>(QDataStream & stream,UserInfoDesc & desc);
};

/*!
 *  @brief 通知消息类型
 *  @details 划分消息通知窗口消息类型
 */
enum NotifyType
{
    NotifySystem,       /*!< 系统级通知消息 */
    NotifyUser,         /*!< 普通用户通知消息 */
    NotifyGroup         /*!< 群组用户通知消息 */
};

/*!
 *  @brief 系统通知信息；
 *  @details 接收到系统发送的请求、聊天等信息，将信息加入通知列表；
 */
struct NotifyInfo
{
    QString identityId;                     /*!< 消息唯一标识 */
    NotifyType type;                        /*!< 消息类型 */
    QString accountId;                      /*!< 通知消息所属发送方用户ID */
    QString nickName;                       /*!< 用户昵称 */
    bool isSystemIcon;                      /*!< 是否为系统图标，默认为true，修改为自定义图标后为false @see UserInfoDesc */
    QString iconId;                         /*!< 图标名称，包含文件后缀：xx.png、xx.jpg等 */

    QString content;                        /*!< 若type为NotifyUser或NotifyGroup时表示聊天内容 */
    MsgCommand msgCommand;                  /*!< 命令类型 */

    OperateFriend ofriendType;              /*!< type为NotifySystem时有效 */
    OperateType stype;                      /*!< 当前请求的类型(人/群) */
    int ofriendResult;                      /*!< 若type为NotifySystem，此结果对应ResponseFriendApply的含义 */

    QString chatId;                         /*!< 群账号(stype为OperateGroup可用) */
    QString chatName;                       /*!< 群名称(stype为OperateGroup可用) */
};

/*!
 *  @brief  联系人分组信息
 */
struct PersonGroupInfo
{
    QString uuid;               /*!< 分组ID */
    QString name;               /*!< 分组名称 */
    int sortNum;                /*!< 分组序号从0开始 */
    bool isDefault;             /*!< 是否为默认分组，一个联系人只有一个默认分组，由服务器在创建用户时指定创建 */
};

/*!
 *  @brief 聊天信息中信息类型
 */
enum ChatMessageType{
    CHAT_C2C,                   /*!< 个人消息 */
    CHAT_C2G,                   /*!< 群消息 */
    CHAT_SYS                    /*!< 系统消息 */
};

/*!
 *  @brief  历史聊天记录信息
 */
struct HistoryChatRecord
{
    HistoryChatRecord():isTop(false),systemIon(true),status(STATUS_OFFLINE){

    }
    QString id;                       /*!< id,int */
    ChatMessageType type;             /*!< 记录类型(群、个人、系统通知) */
    QString accountId;                /*!< 聊天账户 */
    QString nickName;                 /*!< 个人昵称、群名称 */
    qint64 dtime;                     /*!< 日期 */
    QString lastRecord;               /*!< 最后记录 */
    bool isTop;                       /*!< 是否置顶 */
    bool systemIon;                   /*!< 是否为系统头像，默认为系统头像true */
    QString iconId;                   /*!< 图片索引 */
    OnlineStatus status;              /*!< 在线状态 */
};

/*!
 *  @brief 系统设置页面
 */
struct SystemSettingKey
{
    bool autoStartUp;
    bool autoLogin;

    bool keepFront;
    bool exitSystem;
    bool trayIcon;
    bool hidePanel;

    bool windowShaking;

    bool soundAvailable;
    bool lockCheck;
    bool recordCheck;
    bool encryptionCheck;
    bool compressCheck;
};

//++++++++++++++++++++++++++++TextUnit++++++++++++++++++++++++++
enum ShowType{
    Type_OnlyHead = 0,
    Type_NotOnlyHead ,
};
enum BaseTextEditType{
    Type_Default = 0,           //编辑器类型:默认
    Type_UserHead_Friend,       //编辑器类型:用户头
    Type_UserHead_Me ,          //编辑器类型:用户头
    Type_ChatDetail ,           //编辑器类型:聊天内容
    Type_RecordTime ,           //编辑器类型:记录时间
    Type_Tip                    //编辑器类型:提示信息
};

struct UserInfo{
    int id;
    QString name;
    QString head;
};//用户描述

const char ChatFormatType_Text[] = "Text";
const char ChatFormatType_Emoj[] = "Emoj";
const char ChatFormatType_Image[] = "Image";

/*!
 *  @brief 聊天记录内容
 */
struct ChatInfoUnit{

    ChatInfoUnit ():contentType(MSG_TEXT_TEXT){

    }
    QString id;
    MsgCommand contentType;             /*!< 内容类型（html消息/语音消息） */
    QString accountId;                  /*!< 聊天对象账户、群账户 */
    QString nickName;                   /*!< 聊天对象昵称、群名称 */
    qint64 dtime;                       /*!< 日期 */
    QString contents;                   /*!< 聊天内容（html内容/语音文件名称） */
};

enum ParseType{
    Parase_Send,    //解析本地的控件的html
    Parase_Recv     //解析网络端发送来的html
};//聊天数据包解析类型,替换图形为字符串数据

typedef QList<ChatInfoUnit> ChatInfoUnitList;

const QString _Sql_UserList_TableName_ = "UserList";
const QString _Sql_UserList_UserId_ = "UserId";
const QString _Sql_UserList_UserName_ = "UserName";

const QString _Sql_User_TableName_ = "User_";
const QString _Sql_User_UserId_ = "UserId";
const QString _Sql_User_UserName_ = "UserName";
const QString _Sql_User_Userhead_ = "Userhead";
const QString _Sql_User_RecordTime_ = "RecordTime";
const QString _Sql_User_RecordContents_ = "RecordContents";
const QString _Sql_User_RecordTxt_ = "RecordTxt";
const QString _Sql_User_Rowid_ = "rowid";

const unsigned short DefaultQueryRow = 3;  //默认查找的记录数

//++++++++++++++++++++++++++++TextUnit++++++++++++++++++++++++++

#endif // DATASTRUCT_H
