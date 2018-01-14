#include "database.h"

#include <QSqlError>

#include "Util/rutil.h"
#include "Util/rlog.h"

Database::Database(const QString &type, QString connectionName):
    m_open(false)
{
    if(connectionName.size() == 0)
    {
        connectionName = RUtil::UUID();
    }
    database = QSqlDatabase::addDatabase(type,connectionName);
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

bool Database::open()
{
    if(!m_open && !(m_open = database.open()))
    {
        RLOG_ERROR("connect database error! [%s]",database.lastError().text().toLocal8Bit().data());
        return false;
    }
    RLOG_INFO("Connect database host!");

    return m_open;
}
