#include "userinfofile.h"

#include <QDataStream>
#include <QApplication>

#include "Core/constants.h"
#include "Core/head.h"

UserInfoFile::UserInfoFile():
    QObject()
{
    fileName = qApp->applicationDirPath() + QString(Constant::PATH_UserPath) + "/users.bin";
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
        {
            return false;
        }

        if(magic != RIM_USER_FILE_MAGIC)
        {
            return false;
        }

        while(!dataStream.atEnd())
        {
            UserInfoDesc * desc = new UserInfoDesc;
            dataStream>>(*desc);
            users.append(desc);
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
        {
            dataStream<<(*desc);
        }

        return file.flush();

    }
    return false;
}
