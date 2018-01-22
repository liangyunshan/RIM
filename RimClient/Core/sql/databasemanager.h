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

class DatabaseManager
{
public:
    DatabaseManager();
    static DatabaseManager* Instance();
    void setDatabaseType(const QString dtype);
    void setConnectInfo(const QString host,const QString dbName,const QString user,const QString pass,const int port = 0);

    Database * newDatabase(QString connectionName = "");
    Database *getLastDB(){
        return p_DB;
    }

    bool createTablebUserList();
    bool queryUser(int tgtUserId);
    bool insertTgtUser(int tgtUserId,QString name);
    bool createTableUser_id(int tgtUserId);
    bool insertTableUserChatInfo(TextUnit::ChatInfoUnit unit);
    QString querryRecords(int userid, int currRow, int rows= TextUnit::DefaultQueryRow);

    QStringList availableDrivers();

private:
    QString m_hostName;
    QString m_dbName;
    QString m_dbUser;
    QString m_dbPass;

    QString m_dbType;

    unsigned short m_port;
    unsigned short m_QueryRow;
    Database * p_DB;

};

#endif // DATABASEMANAGER_H
