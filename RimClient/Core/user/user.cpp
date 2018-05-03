#include "user.h"

#include <QDir>
#include <QSettings>
#include <QDebug>

#include "constants.h"
#include "Util/rutil.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"

User::User(const UserBaseInfo &baseInfo):userBaseInfo(baseInfo),chatDatabase(nullptr),userSettings(nullptr)
{
    createUserHome(baseInfo.accountId);
    systemSetting = new SystemSettingKey;
}

User::User(const QString userId)
{
    createUserHome(userId);
}

bool User::createUserHome(const QString id)
{
    userHome = QDir::currentPath() + QDir::separator() + Constant::PATH_UserPath + QDir::separator() + id;
    userDBPath = userHome + QDir::separator() + Constant::USER_DBDirName;
    userFilePath = userHome + QDir::separator() + Constant::USER_RecvFileDirName;
    chatImgPath = userFilePath + QDir::separator() + Constant::USER_ChatImageDirName;   //创建当前登录id聊天对话图片保存路径
    if(RUtil::createDir(userHome))
        if(RUtil::createDir(userDBPath))
            if(RUtil::createDir(userFilePath))
                if(RUtil::createDir(chatImgPath))
                return true;

    return  false;
}

User::~User()
{

}

QString User::getUserHome() const
{
    return userHome;
}

/*!
 * @brief 获取登陆用户数据库目录
 * @return 数据库路径
 */
QString User::getUserDatabasePath()
{
    return userDBPath;
}

void User::setUserDatabasePath(const QString &path)
{
    userDBPath = path;
}

/*!
 * @brief 获取登陆用户接收文件路径
 * @return 接收文件路径
 */
QString User::getFileRecvPath()
{
    return userFilePath;
}

void User::setFileRecvPath(const QString &path)
{
    userFilePath = path;
}

QSettings *User::getSettings()
{
    if(!userSettings)
    {
        userSettings = new QSettings(userHome + QDir::separator() + Constant::USER_ConfigName,QSettings::IniFormat);
    }

    return userSettings;
}

/*!
 * @brief 获取指定组对应的key值
 * @param[in] group 分组
 * @param[in] key 键值
 * @param[in] defaultValue 默认值
 * @return 对应key值
 */
QVariant User::getSettingValue(const QString &group, const QString &key, QVariant defaultValue)
{
    if(userSettings)
    {
        userSettings->beginGroup(group);
        QVariant tmp = userSettings->value(key,defaultValue);
        userSettings->endGroup();
        return tmp;
    }
    return defaultValue;
}

void User::setSettingValue(const QString &group, const QString &key, QVariant value)
{
    Q_ASSERT_X(userSettings != nullptr,__FUNCTION__,"get user setting error!");
    userSettings->beginGroup(group);
    userSettings->setValue(key,value);
    userSettings->endGroup();
}

/*!
 * @brief 获取接收目录下指定文件的全路径
 * @param[in] fileId 文件ID
 * @return 是否插入成功
 */
QString User::getFilePath(QString fileId)
{
    QString tmp = userFilePath + QDir::separator() + fileId;
    return tmp;
}

/*!
 * @brief 获取当前用户的ICON信息，判断是系统默认还是用户自定义分别返回
 * @return 返回ICON全路径信息
 */
QString User::getIcon()
{
    return getIcon(userBaseInfo.isSystemIcon,userBaseInfo.iconId);
}

/*!
 * @brief 获取图标路径
 * @param[in] isSystemIcon 是否为系统图标
 * @param[in] iconId 图标名称
 * @return 图标路径
 */
QString User::getIcon(bool isSystemIcon, const QString &iconId)
{
    QString tmpIconPath;
    if(isSystemIcon){
        tmpIconPath = RSingleton<ImageManager>::instance()->getSystemUserIcon(iconId);
    }else{
        tmpIconPath = getFileRecvPath() + QDir::separator() + iconId;
    }

    QFileInfo fileInfo(tmpIconPath);
    if(fileInfo.exists())
        return(tmpIconPath);
    else
        return RSingleton<ImageManager>::instance()->getSystemUserIcon();
}

/*!
 * @brief User::getChatImgPath 获取保存聊天图片的文件夹路径
 * @return 返回当前登录用户的聊天图片文件夹路径
 */
QString User::getChatImgPath()
{
    return chatImgPath;
}

/*!
 * @brief User::setChatImgPath 设置保存聊天图片的文件夹路径
 * @param path 待设置的保存聊天图片的文件夹路径
 */
void User::setChatImgPath(const QString &path)
{
    chatImgPath = path;
}

void User::setDatabase(Database *database)
{
    chatDatabase = database;
}

Database *User::database()
{
    Q_ASSERT_X(chatDatabase != nullptr,__FUNCTION__,"chat database is null");
    return chatDatabase;
}

void User::setTextOnline(bool flag)
{
    textOnLine = flag;
}

bool User::isTextOnLine()
{
    return textOnLine;
}

void User::setFileOnline(bool flag)
{
    fileOnLine = flag;
}

bool User::isFileOnLine()
{
    return fileOnLine;
}
