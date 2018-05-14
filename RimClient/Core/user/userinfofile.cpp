#include "userinfofile.h"

#include <QDataStream>
#include <QApplication>
#include <QDir>
#include <algorithm>

#include "Core/constants.h"
#include "Core/head.h"
#include "Util/rutil.h"

UserInfoFile::UserInfoFile():
    QObject()
{
    QString basePath = qApp->applicationDirPath() + QDir::separator()+ QString(Constant::PATH_UserPath);
    RUtil::createDir(basePath);
    fileName =  basePath + "/users.bin";
}

bool UserInfoFile::readUsers(QList<UserInfoDesc *> &users)
{
    QFile file;

    file.setFileName(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QDataStream dataStream(&file);

        int version;
        int magic;

        dataStream>>version>>magic;

        if(version < RIM_VERSION)
            return false;

        if(magic != RIM_USER_FILE_MAGIC)
            return false;

        while(!dataStream.atEnd())
        {
            UserInfoDesc * desc = new UserInfoDesc;
            dataStream>>(*desc);

            auto findeIndex = std::find_if(users.begin(),users.end(),[&desc](UserInfoDesc * tmps){
                return tmps->accountId == desc->accountId;
            });

            if(findeIndex == users.end()){
                users.append(desc);
            }
        }
    }
    return false;
}

bool UserInfoFile::saveUsers(const QList<UserInfoDesc *> users)
{
    QFile file;
    file.setFileName(fileName);
    if(file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QDataStream dataStream(&file);

        dataStream<<RIM_VERSION<<RIM_USER_FILE_MAGIC;

        foreach(UserInfoDesc * desc,users)
            dataStream<<(*desc);

        return file.flush();
    }
    return false;
}
