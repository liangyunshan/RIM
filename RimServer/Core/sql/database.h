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

class Database
{
public:
    Database(const QString& type, QString connectionName = "");

    void setDatabaseName(const QString& name);
    void setUserName(const QString& name);
    void setPassword(const QString& password);
    void setHostName(const QString& host);
    void setPort(int port);

    QString connectionName() const;

    bool open();

private:
    QSqlDatabase database;
    bool m_open;

};

#endif // DATABASE_H
