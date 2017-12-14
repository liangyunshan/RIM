/*!
 *  @brief     数据定义
 *  @details   定义了软件运行时所有的结构体、枚举等
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright GNU Public License.
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


#endif // DATASTRUCT_H
