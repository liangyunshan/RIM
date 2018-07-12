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

Database DatabaseManager::database(QString connectionName)
{
    Database db(m_dbType,connectionName);
    if(db.init())
    {
        db.setHostName(m_hostName);
        db.setDatabaseName(m_dbName);
        db.setUserName(m_dbUser);
        db.setPassword(m_dbPass);

        db.open();
    }
    return db;
}

/*!
 * @brief 查询当前数据库是否支持指定的功能
 * @param[in] feature 待查询的功能
 * @return 是否支持指定的功能
 */
bool DatabaseManager::hasFeature(QSqlDriver::DriverFeature feature)
{
    static Database db(m_dbType);
    if(db.init()){
        return db.sqlDatabase().driver()->hasFeature(feature);
    }
    return false;
}

QStringList DatabaseManager::availableDrivers()
{
    return QSqlDatabase::drivers();
}
