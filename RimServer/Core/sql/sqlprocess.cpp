#include "sqlprocess.h"

#include <QString>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QMutex>
#include <QDate>

#include "datatable.h"
#include "sql/database.h"
#include "Util/rutil.h"
#include "constants.h"

QMutex ACCOUNT_LOCK;

int G_BaseAccountId = 0;

SQLProcess::SQLProcess()
{

}

ResponseRegister SQLProcess::processUserRegist(Database *db, const RegistRequest *request, QString &id)
{
    DataTable::RimConfig config;
    bool sqlError = true;
    ACCOUNT_LOCK.lock();

    if(G_BaseAccountId == 0)
    {
        //Yang 待优化对数据库操作的方式
        QString sql = QString("select %1 from %2 where %3 = '%4'").arg(config.value)
                .arg(config.table).arg(config.name).arg(config.accuoutId);
        QSqlQuery query(db->sqlDatabase());
        if(query.exec(sql))
        {
            if(query.next())
            {
               G_BaseAccountId = query.value(0).toInt();
               sqlError = REGISTER_FAILED;
            }
        }
    }
    else
    {
        sqlError = false;
    }

    G_BaseAccountId++;

    ACCOUNT_LOCK.unlock();

    if(sqlError)
    {
        return REGISTER_FAILED;
    }

    DataTable::User user;
    QString sql = QString("insert into %1 (%2,%3,%4,%5) values ('%6','%7','%8','%9')").arg(user.table)
            .arg(user.id).arg(user.account).arg(user.password).arg(user.nickName)
            .arg(RUtil::UUID()).arg(QString::number(G_BaseAccountId)).arg(request->password).arg(request->nickName);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        id  = QString::number(G_BaseAccountId);

        QString sql = QString("update %1 set %2 = %3 where %4 = '%5'")
                .arg(config.table).arg(config.value).arg(G_BaseAccountId)
                .arg(config.name).arg(config.accuoutId);
        if(query.exec(sql))
        {

        }

        return REGISTER_SUCCESS;
    }

    return REGISTER_FAILED;
}

ResponseLogin SQLProcess::processUserLogin(Database * db,const LoginRequest *request)
{
    DataTable::User user;
    QString sql = QString("select %1 from %2 where %3 = '%4'")
            .arg(user.password).arg(user.table).arg(user.account).arg(request->accountId);

    QSqlQuery query(db->sqlDatabase());

    if(query.exec(sql))
    {
        if(query.next())
        {
             if(query.value(0).toString() != request->password)
             {
                    return LOGIN_PASS_ERROR;
             }
             else
             {
                    return LOGIN_SUCCESS;
             }
        }
        else
        {
            return LOGIN_UNREGISTERED;
        }
    }

    return LOGIN_FAILED;
}

ResponseUpdateUser SQLProcess::processUpdateUserInfo(Database *db, const UpdateBaseInfoRequest *request)
{
    DataTable::User user;
    QString sql = QString("update %1 set %2 = '%3' where %4 = '%5' ")
            .arg(user.table).arg(user.nickName).arg(request->baseInfo.nickName).arg(user.account).arg(request->baseInfo.accountId);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        return UPDATE_USER_SUCCESS;
    }

    return UPDATE_USER_FAILED;
}

void SQLProcess::getUserInfo(Database *db,const QString accountId, UserBaseInfo & userInfo)
{
    DataTable::User user;
    QString sql = QString("select * from %1 where %2 = '%3' ").arg(user.table).arg(user.account).arg(accountId);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        if(query.next())
        {
            userInfo.accountId = accountId;
            userInfo.nickName = query.value(user.nickName).toString();
            userInfo.signName = query.value(user.signName).toString();
            userInfo.sexual = (Sexual)query.value(user.gender).toInt();
            userInfo.birthday = query.value(user.birthDay).toDate().toString(Constant::Date_Simple);
            userInfo.address = query.value(user.address).toString();
            userInfo.email = query.value(user.email).toString();
            userInfo.phoneNumber = query.value(user.phone).toString();
            userInfo.desc = query.value(user.desc).toString();
            userInfo.face = query.value(user.face).toInt();
            userInfo.customImgId = query.value(user.faceId).toString();
        }
    }
}

