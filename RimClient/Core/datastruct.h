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
    MESS_SETTINGS                //系统设置修改
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

namespace FriendList {
    /*!
      @brief 好友状态
    */
    enum UserState
    {
        STATE_ONLINE,       //在线
        STATE_OFFLINE,      //离线
        STATE_BUSY,         //忙碌
        STATE_HIDE,         //隐身
        STATE_NODISTURB,    //请勿打扰
    };

    /*!
      @brief 好友信息
    */
    struct FriendInfo
    {
        QString account;    //账户
        QString nickName;   //昵称
        QString remark;     //备注
        QString signName;   //签名
        QString headId;     //头像
        UserState state;    //在线状态
    };

    /*!
      @brief 分组信息
    */
    struct GroupInfo
    {
        bool  ifFold;                   //当前是否折叠
        QString groupName;              //分组名称
        QList <FriendInfo> contents;    //分组中内容
    };
}


#endif // DATASTRUCT_H
