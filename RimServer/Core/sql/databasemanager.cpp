#include "databasemanager.h"

#include <QSqlError>

#include "Util/rlog.h"
#include "Util/rutil.h"

DatabaseManager::DatabaseManager():
    m_hostName(""),m_dbName(""),m_dbUser(""),m_dbPass(""),m_port(0)
{

}

void DatabaseManager::setDatabaseType(Datastruct::DatabaseType type)
{
    m_dbType = type;
}

void DatabaseManager::setConnectInfo(const QString host, const QString dbName, const QString user, const QString pass, const int port)
{
    m_hostName = host;
    m_dbName = dbName;
    m_dbUser = user;
    m_dbPass = pass;
    m_port = port;
}

Database *DatabaseManager::newDatabase(QString connectionName)
{
    Database * db = new Database(m_dbType,connectionName);
    if(db->init())
    {
        db->setHostName(m_hostName);
        db->setDatabaseName(m_dbName);
        db->setUserName(m_dbUser);
        db->setPassword(m_dbPass);

        db->open();
    }
    return db;
}

QStringList DatabaseManager::availableDrivers()
{
    return QSqlDatabase::drivers();
}
