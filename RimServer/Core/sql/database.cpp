#include "database.h"

#include <QSqlError>
#include <QDebug>

#include "Util/rutil.h"
#include "Util/rlog.h"

Database::Database(Datastruct::DatabaseType type, QString connectionName):
    m_open(false),m_error(false),dbType(type)
{
    dbId = connectionName.size() == 0 ?connectionName = RUtil::UUID():connectionName;
}

/*!
 * @brief 加载对应类型数据库驱动
 * @param[in] 无
 * @return 数据库驱动是否加载成功
 * @note 目前支持的数据库可参考 @link DatabaseType @endlink
 */
bool Database::init()
{
    QString dbDriver;
    switch(dbType)
    {
        case Datastruct::DB_ORACLE:
            dbDriver = "QORACLE";
            break;
        case Datastruct::DB_MYSQL :
        default:
            dbDriver = "QMYSQL";
            break;
    }

    database = QSqlDatabase::addDatabase(dbDriver,dbId);
    QSqlError::ErrorType errorType = database.lastError().type();
    if(errorType != QSqlError::NoError)
    {
        RLOG_ERROR(database.lastError().text().toLocal8Bit().data());
        m_error = true;
        return false;
    }

    return true;
}

void Database::setDatabaseName(const QString &name)
{
    database.setDatabaseName(name);
}

void Database::setUserName(const QString &name)
{
    database.setUserName(name);
}

void Database::setPassword(const QString &password)
{
    database.setPassword(password);
}

void Database::setHostName(const QString &host)
{
    database.setHostName(host);
}

void Database::setPort(int port)
{
    if(port > 0)
    {
        database.setPort(port);
    }
}

QString Database::connectionName() const
{
    return database.connectionName();
}

QString Database::errorInfo() const
{
    if(database.lastError().type() != QSqlError::NoError)
    {
        return database.lastError().text();
    }

    return "";
}

bool Database::open()
{
    if(!m_open && !(m_open = database.open()))
    {
        RLOG_ERROR("connect database error! [%s]",database.lastError().text().toLocal8Bit().data());
        m_error = true;
        return false;
    }
    RLOG_INFO("Connect database host!");

    return m_open;
}

bool Database::isError() const
{
    return m_error;
}
