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

/*!
    @note  条件查询可分为多种组合，如from table where a = 1 and b != 3 or c like '%a%';
    分析sql语句，可将条件分为两种:
    [1]and、or 用于连接多个条件
    [2]=、!=、like、in 用于具体描述某个条件

    类Criteria用于连接多个条件，可通过add()、or()方式对条件进行拼接；
    类Restrictions用于描述具体条件，可通过eq()、gt()、lt()、like()、unlike()、in()等设置不同的条件；

    通过类Criteria可返回最终的条件部分sql语句
*/

class Restrictions
{
public:
    static Restrictions eq(QString name,QVariant value);
    static Restrictions gt(QString name,QVariant value);
    static Restrictions ge(QString name,QVariant value);
    static Restrictions lt(QString name,QVariant value);
    static Restrictions le(QString name,QVariant value);
    static Restrictions ne(QString name,QVariant value);
    static Restrictions like(QString name,QVariant value);
    static Restrictions in(QString name,QVariant value);

    bool operator<(const Restrictions & src)const;

    QString toSql()const;
private:
    enum OperateType
    {
        EQ,             //=
        GT,             //>
        GE,             //>=
        LT,             //<
        LE,             //<=
        NE,             //!=
        LIKE,           //like
        IN              //in
    };

    explicit Restrictions(QString name,QVariant value,OperateType type);

    QString name;
    QVariant value;

    OperateType operation;
};

class Criteria
{
public:
    explicit Criteria();
    Criteria & add(Restrictions rest);
    Criteria & orr(Restrictions rest);

    int size(){return restricitinons.size();}

    void clear(){restricitinons.clear();}

    QString toSql();

private:
    enum CriteriaType
    {
        CADD,
        COR
    };

    QMap<Restrictions,CriteriaType> restricitinons;
};

class SuperCondition
{
public:
    SuperCondition(){}

    Criteria& createCriteria()
    {
        return ctia;
    }

    void clearRestrictions()
    {
        ctia.clear();
    }

    virtual QString sql() = 0;

    Criteria ctia;
};

class RSelect : public SuperCondition
{
public:
    explicit RSelect(const QString tableName);
    void select(const QString key);

    QString sql();

private:
    QString tableName;
    QList<QString> selectKeys;
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

class RUpdate : public SuperCondition
{
public:
    explicit RUpdate(const QString tableName);
    void update(const QString key,QVariant value);
    QString sql();

private:
    QString tableName;
    QMap<QString,QVariant> maps;
};

class RDelete  : public SuperCondition
{
public:
    explicit RDelete(const QString tableName);
    QString sql();

private:
    QString tableName;
};

#endif // RPERSISTENCE_H
