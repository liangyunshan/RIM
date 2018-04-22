/*!
 *  @brief     数据库管理
 *  @details   负责创建数据库连接
 *  @file      databasemanager.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.XX
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>

#include "database.h"
#include "datastruct.h"

#define SQL_SQLITE "SQLITE"
#define SQL_MYSQL "MYSQL"

class DatabaseManager
{
public:
    DatabaseManager();
    static DatabaseManager* Instance();
    void setDatabaseType(const QString dtype);
    void setConnectInfo(const QString host,const QString dbName,const QString user,const QString pass,const int port = 0);

    bool testSupportDB(QString dbtype);
    Database * newDatabase(QString connectionName = "");

    QStringList availableDrivers();

private:
    QString m_hostName;
    QString m_dbName;
    QString m_dbUser;
    QString m_dbPass;

    QString m_dbType;

    unsigned short m_port;
    unsigned short m_QueryRow;
};

#endif // DATABASEMANAGER_H
