/*!
 *  @brief     用户信息文件
 *  @details   记录、解析用户登录信息
 *  @file      userinfofile.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.12
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef USERINFOFILE_H
#define USERINFOFILE_H

#include <QObject>
#include <QFile>

class UserInfoDesc;

class UserInfoFile : public QObject
{
    Q_OBJECT
public:
    UserInfoFile();

    bool readUsers(QList<UserInfoDesc *> & users);
    bool saveUsers(const QList<UserInfoDesc *> users);

private:
    QString fileName;
};

#endif // USERINFOFILE_H
