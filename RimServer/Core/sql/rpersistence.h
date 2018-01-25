/*!
 *  @brief     SQL组装
 *  @details   实现对SQL语句的组装，增加灵活性
 *  @file      rpersistence.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.24
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RPERSISTENCE_H
#define RPERSISTENCE_H

#include <QString>
#include <QVariant>
#include <QMap>
#include <QList>

class RSelect
{
public:
    explicit RSelect(const QString tableName);
    void select(const QString key);
    void addCondition(const QString key,QVariant value);
    QString sql();

private:
    QString tableName;
    QList<QString> selectKeys;
    QMap<QString,QVariant> conditions;
};

class RPersistence
{
public:
    explicit RPersistence(const QString tableName);
    void insert(const QString key,QVariant value);
    QString sql();

private:
    QString tableName;
    QMap<QString,QVariant> maps;
};

class RUpdate
{
public:
    explicit RUpdate(const QString tableName);
    void update(const QString key,QVariant value);
    void addCondition(const QString key,QVariant value);
    QString sql();

private:
    QString tableName;
    QMap<QString,QVariant> maps;
    QMap<QString,QVariant> conditions;
};

class RDelete
{
public:
    explicit RDelete(const QString tableName);
    void addCondition(const QString key,QVariant value);
    QString sql();

private:
    QString tableName;
    QMap<QString,QVariant> conditions;
};

#endif // RPERSISTENCE_H
