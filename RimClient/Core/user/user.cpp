#include "user.h"

#include <QDir>
#include <QSettings>
#include <QDebug>

#include "constants.h"
#include "Util/rutil.h"
#include "util/rsingleton.h"
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

void User::createUserHome(const QString id)
{
    userHome = QDir::currentPath() + QDir::separator() + Constant::PATH_UserPath + QDir::separator() + id;
    userDBPath = userHome + QDir::separator() + Constant::USER_DBDirName;
    userFilePath = userHome + QDir::separator() + Constant::USER_RecvFileDirName;
    chatFilePath = userFilePath + QDir::separator() + Constant::USER_ChatFileDirName;
    chatImgPath = userFilePath + QDir::separator() + Constant::USER_ChatImageDirName;
    audioPath = userFilePath + QDir::separator() + Constant::USER_ChatAudioDirName;
    if(RUtil::createDir(userHome)){
        RUtil::createDir(userDBPath);
        if(RUtil::createDir(userFilePath)){
            if(RUtil::createDir(chatFilePath)){
                RUtil::createDir(chatFilePath + QDir::separator() + Constant::USER_C2CDirName);
                RUtil::createDir(chatFilePath + QDir::separator() + Constant::USER_GroupDirName);
            }
            if(RUtil::createDir(chatImgPath)){
                RUtil::createDir(chatImgPath + QDir::separator() + Constant::USER_C2CDirName);
                RUtil::createDir(chatImgPath + QDir::separator() + Constant::USER_GroupDirName);
            }
            if(RUtil::createDir(audioPath)){
                RUtil::createDir(audioPath + QDir::separator() + Constant::USER_C2CDirName);
                RUtil::createDir(audioPath + QDir::separator() + Constant::USER_GroupDirName);
            }
        }
    }
}

User::~User()
{

}

QString User::getUserHome() const
{
    RUtil::createDir(userHome);
    return userHome;
}

/*!
 * @brief 获取登陆用户数据库目录
 * @return 数据库路径
 */
QString User::getUserDatabasePath()
{
    RUtil::createDir(userDBPath);
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
    RUtil::createDir(userFilePath);
    return userFilePath;
}

void User::setFileRecvPath(const QString &path)
{
    userFilePath = path;
}

QString User::getC2CImagePath()
{
    QString tmpPath = chatImgPath + QDir::separator() + Constant::USER_C2CDirName;
    RUtil::createDir(tmpPath);
    return tmpPath;
}

QString User::getGroupImagePath()
{
    QString tmpPath = chatImgPath + QDir::separator() + Constant::USER_GroupDirName;
    RUtil::createDir(tmpPath);
    return tmpPath;
}

QString User::getC2CAudioPath()
{
    QString tmpPath = audioPath + QDir::separator() + Constant::USER_C2CDirName;
    RUtil::createDir(tmpPath);
    return tmpPath;
}

QString User::getGroupAudioPath()
{
    QString tmpPath = audioPath + QDir::separator() + Constant::USER_GroupDirName;
    RUtil::createDir(tmpPath);
    return tmpPath;
}

QString User::getC2CFilePath()
{
    QString tmpPath = chatFilePath + QDir::separator() + Constant::USER_C2CDirName;
    RUtil::createDir(tmpPath);
    return tmpPath;
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
QString User::getFilePath(QString fileId, ChatT group, ChatK type)
{
    if(type == C_Image){
        if(group == C_C2C){
            return getC2CImagePath() + QDir::separator() + fileId;
        }else if(group == C_GROUP){
            return getGroupImagePath() + QDir::separator() + fileId;
        }
    }else if(type == C_Audio){
        if(group == C_C2C){
            return getC2CAudioPath() + QDir::separator() + fileId;
        }else if(group == C_GROUP){
            return getGroupAudioPath() + QDir::separator() + fileId;
        }
    }
    return QString();
}

/*!
 * @brief 获取当前用户的ICON信息，判断是系统默认还是用户自定义分别返回
 * @return 返回ICON全路径信息
 */
QString User::getIcon(ChatT group)
{
    return getIcon(userBaseInfo.isSystemIcon,userBaseInfo.iconId,group);
}

/*!
 * @brief 获取图标路径
 * @param[in] isSystemIcon 是否为系统图标
 * @param[in] iconId 图标名称
 * @return 图标路径
 */
QString User::getIcon(bool isSystemIcon, const QString &iconId, ChatT group)
{
    QString tmpIconPath;
    if(isSystemIcon){
        tmpIconPath = RSingleton<ImageManager>::instance()->getSystemUserIcon(iconId);
    }else{
        if(group == C_C2C){
            tmpIconPath = getC2CImagePath() + QDir::separator() + iconId;
        }else if(group == C_GROUP){
            tmpIconPath = getGroupImagePath() + QDir::separator() + iconId;
        }
    }

    QFileInfo fileInfo(tmpIconPath);
    if(fileInfo.isFile() && fileInfo.exists())
        return(tmpIconPath);
    else
        return RSingleton<ImageManager>::instance()->getSystemUserIcon();
}

/*!
 * @brief User::getIconAbsoultePath 获取头像的全路径
 * @param isSystemIcon 是否为系统图标
 * @param iconId 图标名称
 * @return 图标路径
 */
QString User::getIconAbsoultePath(bool isSystemIcon, const QString &iconId)
{
    QString tmpIconPath;
    QDir t_parentDir = QDir::current();
    t_parentDir.cdUp();
    QString t_userPath = t_parentDir.path() + QDir::separator() + Constant::PATH_UserDirName + QDir::separator() + userBaseInfo.accountId;
    QString t_userRecvFilePath = t_userPath + QDir::separator() + Constant::USER_RecvFileDirName;
    QString t_userC2CPath = t_userRecvFilePath + QDir::separator() + Constant::USER_ChatImageDirName +QDir::separator() + Constant::USER_C2CDirName;

    if(isSystemIcon)
    {
        tmpIconPath = RSingleton<ImageManager>::instance()->getSystemUserIcon(iconId);
    }
    else
    {
        tmpIconPath = t_userC2CPath + QDir::separator() + iconId;
    }

    QFileInfo fileInfo(tmpIconPath);
    if(fileInfo.exists())
        return(QDir::fromNativeSeparators(tmpIconPath));
    else
        return QDir::fromNativeSeparators(RSingleton<ImageManager>::instance()->getSystemUserIcon());
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

void User::setLogin(bool flag)
{
    userLogined = flag;
}

bool User::isLogin()
{
    return userLogined;
}

void User::setUserInfoDesc(UserInfoDesc &desc)
{
    this->userLoginBaseInfo = desc;
}

UserInfoDesc User::getUserInfoDesc()
{
    return this->userLoginBaseInfo;
}
