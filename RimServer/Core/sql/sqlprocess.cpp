#include "sqlprocess.h"

#include <QString>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QMutex>

#include "datatable.h"
#include "sql/database.h"
#include "Util/rutil.h"

QMutex ACCOUNT_LOCK;

int G_BaseAccountId = 0;

SQLProcess::SQLProcess()
{

}

bool SQLProcess::processUserRegist(Database *db, RegistRequest *request, QString &id)
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
               sqlError = false;
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
        return false;
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

        return true;
    }

    return false;
}

bool SQLProcess::processUserLogin(Database * db,LoginRequest *request)
{
    DataTable::User user;
    QString sql = QString("select * from %1 where %2 = '%3' and %4 = '%5' ")
            .arg(user.table).arg(user.account).arg(request->accountId).arg(user.password).arg(request->password);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        if(query.next())
        {
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"success";
            return true;
        }
        else
        {
            qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"no record";
        }
    }

    return false;
}
