/*!
 *  @brief     用户信息文件
 *  @details   记录、解析用户登录信息
 *  @file      userinfofile.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.12
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note   20180509:wey:增加读取去重
 */
#ifndef USERINFOFILE_H
#define USERINFOFILE_H

#include <QObject>
#include <QFile>

#include "protocol/datastruct.h"

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
