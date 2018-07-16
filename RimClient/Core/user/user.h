/*!
 *  @brief     登陆用户信息
 *  @details   保存当前登陆用户的所有信息，可通过此获取登陆用户文件目录系统
 *  @file      user.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.03.22
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *       20180322:wey:调整个个人信息访问接口
 *       20180503:wey:调整用户文件、音频保存目录结构
 */
#ifndef USER_H_2018_03_22
#define USER_H_2018_03_22

#include <QString>
#include <QVariant>
class QSettings;
class Database;

#include "../protocol/protocoldata.h"
using namespace ProtocolType;

#include "../protocol/datastruct.h"

class User
{
public:
    explicit User(const UserBaseInfo & baseInfo);
    User(const QString userId);
    ~User();

public:
    enum ChatT{C_C2C,C_GROUP};
    enum ChatK{C_Image,C_Audio};

    QString getUserHome()const;

    QString getUserDatabasePath();
    void setUserDatabasePath(const QString & path);

    QString getFileRecvPath();
    void setFileRecvPath(const QString & path);

    QString getC2CImagePath();
    QString getGroupImagePath();
    QString getC2CAudioPath();
    QString getGroupAudioPath();

    QString getC2CFilePath();

    QSettings * getSettings();

    QVariant getSettingValue(const QString & group,const QString &key,QVariant defaultValue);
    void setSettingValue(const QString & group,const QString &key,QVariant value);

    QString getFilePath(QString fileId,ChatT group = C_C2C,ChatK type = C_Image);
    QString getIcon(ChatT group = C_C2C);
    QString getIcon(bool isSystemIcon,const QString &iconId,ChatT group = C_C2C);
    QString getIconAbsoultePath(bool isSystemIcon,const QString &iconId);

    QString getChatImgPath();
    void setChatImgPath(const QString &path);

    void setDatabase(Database * database);
    Database * database();

    void setTextOnline(bool flag = true);
    bool isTextOnLine();

    void setFileOnline(bool flag = true);
    bool isFileOnLine();

    void setLogin(bool flag = true);
    bool isLogin();

    SystemSettingKey * systemSettings(){return this->systemSetting;}

    UserBaseInfo & BaseInfo(){return userBaseInfo;}

    void setUserInfoDesc(UserInfoDesc &desc);
    UserInfoDesc getUserInfoDesc();

private:
    void createUserHome(const QString id);

private:
    static User * puser;

    UserInfoDesc userLoginBaseInfo;  /*!< 用户登陆时基本信息 */
    UserBaseInfo userBaseInfo;       /*!< 用户基本数据信息 */
    bool textOnLine;                 /*!< 信息服务器在线，发送消息前检测是否在线以及是否登陆 */
    bool fileOnLine;                 /*!< 文件服务器在线 */
    bool userLogined;                /*!< 用户是否登陆 */

    QString userHome;
    QString userDBPath;
    QString userFilePath;
    QString chatFilePath;           /*!< 聊天对话中文件存储路径 */
    QString chatImgPath;            /*!< 聊天对话中图片存储路径 */
    QString audioPath;              /*!< 聊天对话中音频存储路径 */

    QSettings * userSettings;

    Database * chatDatabase;            /*!< 数据库连接 */

    SystemSettingKey * systemSetting;   /*!< 系统设置 */
};

#endif // USER_H
