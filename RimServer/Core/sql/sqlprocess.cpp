#include "sqlprocess.h"

#include <QString>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>

#include "datatable.h"
#include "sql/database.h"

SQLProcess::SQLProcess()
{

}

bool SQLProcess::processUserLogin(Database * db,LoginRequest *request)
{
    DataTable::User tUser;
    QString sql = QString("select * from %1 where %2 = '%3' and %4 = '%5' ")
            .arg(tUser.table).arg(tUser.account).arg(request->accountName).arg(tUser.password).arg(request->password);

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
