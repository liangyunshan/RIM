#include "databasemanager.h"

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#include <algorithm>

#include "Util/rutil.h"

DatabaseManager *p_This = NULL;

DatabaseManager::DatabaseManager():
    m_hostName(""),m_dbName(""),m_dbUser(""),m_dbPass(""),m_port(0),
    m_dbType("")
{
    p_This = this;
    m_QueryRow = TextUnit::DefaultQueryRow;
}

DatabaseManager *DatabaseManager::Instance()
{
    return p_This;
}

void DatabaseManager::setDatabaseType(const QString dtype)
{
    m_dbType = dtype;
}

void DatabaseManager::setConnectInfo(const QString host, const QString dbName, const QString user, const QString pass, const int port)
{
    m_hostName = host;
    m_dbName = dbName;
    m_dbUser = user;
    m_dbPass = pass;
    m_port = port;
}

/*!
 * @brief 测试是否支持该类型数据库驱动
 * @param[in] dbtype 待测试数据库类型
 * @return 支持结果
 */
bool DatabaseManager::testSupportDB(QString dbtype)
{
    QStringList supportDriver = availableDrivers();

    QStringList::iterator iter = std::find_if(supportDriver.begin(),supportDriver.end(),[=](QString driverName){
        return dbtype == driverName;
    });

    if(iter != supportDriver.end())
        return true;

    return false;
}

/*!
 * @brief 创建数据库连接
 * @details 不同的数据库，其创建的方式会存在不一致：
 *          [1]QSQLITE:轻量级数据库，可不需要设置主机等基本信息;
 *          [2]QMYSQL:需要设置主机、用户名、密码、数据库名。
 * @param[in] connectionName 数据库连接名称
 * @return 创建成功的数据库连接或者nullptr
 */
Database *DatabaseManager::newDatabase(QString connectionName)
{
    Database * db = new Database(m_dbType,connectionName);
    db->setHostName(m_hostName);
    db->setDatabaseName(m_dbName);
    db->setUserName(m_dbUser);
    db->setPassword(m_dbPass);

    if(db->open())
        return db;

    delete db;
    return nullptr;
}

QStringList DatabaseManager::availableDrivers()
{
    return QSqlDatabase::drivers();
}
