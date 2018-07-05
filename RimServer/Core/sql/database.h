/*!
 *  @brief     数据库连接
 *  @details
 *  @file      database.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>

#include "protocol/datastruct.h"

class Database
{
public:
    Database(Datastruct::DatabaseType type, QString connectionName = "");

    bool init();
    void setDatabaseName(const QString& name);
    void setUserName(const QString& name);
    void setPassword(const QString& password);
    void setHostName(const QString& host);
    void setPort(int port);

    QSqlDatabase& sqlDatabase(){return database;}

    QString connectionName() const;
    QString errorInfo()const;

    bool open();
    bool isError()const;

private:
    QSqlDatabase database;
    Datastruct::DatabaseType dbType;
    QString dbId;           /*!< 数据库连接标识 */
    bool m_error;           /*!< 是否存在错误 */
    bool m_open;            /*!< 数据库是否开启 */
};

#endif // DATABASE_H
