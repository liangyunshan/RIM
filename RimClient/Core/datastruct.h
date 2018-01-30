/*!
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
 *  @brief  消息种类
 */
enum MessageType
{
    MESS_STYLE,                  //样式更新
    MESS_SHORTCUT,               //快捷键更新
    MESS_SETTINGS,               //系统设置修改
    MESS_BASEINFO_UPDATE         //基本信息修改
};

/*!
  @brief 用户描述信息
*/
struct UserInfoDesc
{
    UserInfoDesc()
    {
        isRemberPassword = false;
        isAutoLogin = false;
        isSystemPixMap = false;
    }
    QString userName;               /*!< 用户名(暂不支持中文) */
    int loginState;                 /*!< 登录状态，参见OnLineState::UserState枚举*/
    QString originPassWord;         /*!< 原始密码 */
    QString password;               /*!< 加密后的密码 */
    bool isRemberPassword;          /*!< 是否记住密码 */
    bool isAutoLogin;               /*!< 是否自动登录 */
    bool isSystemPixMap;            /*!< 是否为系统默认头像 */
    QString pixmap;                 /*!< 头像路径 */

    friend QDataStream & operator <<(QDataStream & stream,const UserInfoDesc & desc);
    friend QDataStream & operator >>(QDataStream & stream,UserInfoDesc & desc);
};

/*!
 *  @brief 通知消息类型
 */
enum NotifyType
{
    NotifySystem,
    NotifyUser,
    NotifyGroup
};

/*!
 *  @brief 系统通知信息；
 *  @details 接收到系统发送的请求、聊天等信息，将信息加入通知列表；
 */
struct NotifyInfo
{
    QString identityId;                     //消息唯一标识
    NotifyType type;
    QString accountId;                      //对方ID
    QString nickName;
    unsigned short face;                    //头像信息(0表示为自定义，大于0表示系统头像)
    QString pixmap;

    QString content;                        //type为NotifyUser或NotifyGroup时表示聊天内容

    OperateFriend ofriendType;              //type为NotifySystem时，可用
    SearchType stype;                       //当前请求的类型(人/群)
    int ofriendResult;                      //ResponseFriendApply

};

const int TestUserId = 123456;
namespace TextUnit {
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

    struct UserChatFont{
        QString fontName;
        qint32  fontSize;
        bool    fontIsBold;
        bool    fontIsItalic;
        bool    fontIsUnderline;
        qint64  fontColorRGBA;

        UserChatFont (){
            fontName = "Unknown";
            fontSize = 0;
            fontIsBold = false;
            fontIsItalic = false;
            fontIsUnderline = false;
            fontColorRGBA = 0x00000000;
        }
    };//聊天内容字体描述

    struct ChatInfoUnit{
        UserInfo user;
        QString time;
        UserChatFont font;
        QString contents;

        ChatInfoUnit (){
            user.name = "Unknown";
            user.head = "Unknown";
            time = "";
            contents = "";
        }
    };//一条聊天记录描述

    enum ParseType{
        Parase_Send,    //解析本地的控件的html
        Parase_Recv     //解析网络端发送来的html
    };//聊天数据包解析类型,替换图形为字符串数据

    typedef QList<ChatInfoUnit> ChatInfoUnitList;

    const QString _Sql_Table_01_Name_ = "UserList";
    const QString _Sql_UserList_UserId_ = "UserId";
    const QString _Sql_UserList_UserName_ = "UserName";

    const QString _Sql_Table_02_Name_ = "User_";
    const QString _Sql_User_UserId_ = "UserId";
    const QString _Sql_User_UserName_ = "UserName";
    const QString _Sql_User_Userhead_ = "Userhead";
    const QString _Sql_User_RecordTime_ = "RecordTime";
    const QString _Sql_User_RecordContents_ = "RecordContents";
    const QString _Sql_User_RecordTxt_ = "RecordTxt";

    const unsigned short DefaultQueryRow = 15;  //默认查找的记录数
}

namespace GroupPerson {
    struct PersonGroupInfo{
        QString uuid;
        QString name;
        int sortNum;
    };//联系人分组信息
}


#endif // DATASTRUCT_H
