#include "rpersistence.h"

#include <QDebug>

QString parseMap(QMap<QString,QVariant> & maps)
{
    QString result;
    QList<QString> keys = maps.keys();
    for(int i = 0; i < keys.size(); i++)
    {
        result += keys.at(i);
        result += " = ";

        QVariant val = maps.value(keys.at(i));

        switch(val.type())
        {
             case QVariant::Date:
             case QVariant::DateTime:
             case QVariant::String:
                                     result += (" '" + val.toString() + "' ");
                                     break;
             default:
                                     result += val.toString();
                                     break;
        }

        if(i < keys.size() - 1)
        {
            result += ", ";
        }
        else
        {
            result += " ";
        }
    }

    return result;
}

QString parseList(QList<QString> & selectKeys)
{
    QString result;
    for(int i  = 0; i < selectKeys.size(); i++)
    {
         result += selectKeys.at(i);
         if(i < selectKeys.size() - 1)
         {
              result += ",";
         }
    }

    return result;
}

RPersistence::RPersistence(const QString tableName):
    tableName(tableName)
{

}

void RPersistence::insert(const QString key, QVariant value)
{
    maps.insert(key,value);
}

QString RPersistence::sql()
{
    QString result = "insert into ";
    result += tableName;
    result += " ( ";

    QList<QString> keys = maps.keys();

    result += parseList(keys);
    result += " ) values ( ";

    for(int i = 0; i < keys.size(); i++)
    {
       QVariant val = maps.value(keys.at(i));

       switch(val.type())
       {
            case QVariant::Date:
            case QVariant::DateTime:
            case QVariant::String:
                                    result += (" '" + val.toString() + "' ");
                                    break;
            default:
                                    result += val.toString();
                                    break;
       }
       if(i < keys.size() - 1)
       {
           result += ", ";
       }
       else
       {
           result += " )";
       }
    }

    return result;
}

RUpdate::RUpdate(const QString tableName):
    tableName(tableName)
{

}

void RUpdate::update(const QString key, QVariant value)
{
    maps.insert(key,value);
}

void RUpdate::addCondition(const QString key, QVariant value)
{
    conditions.insert(key,value);
}

QString RUpdate::sql()
{
    QString result = " update ";
    result += tableName;
    result += " set ";

    result += parseMap(maps);

    if(conditions.size() > 0)
    {
        result += " where ";
        result += parseMap(conditions);
    }
    return result;
}

RSelect::RSelect(const QString tableName):tableName(tableName)
{

}

void RSelect::select(const QString key)
{
    selectKeys.append(key);
}

void RSelect::addCondition(const QString key, QVariant value)
{
    conditions.insert(key,value);
}

QString RSelect::sql()
{
    QString result = " select ";

    if(selectKeys.size() == 0)
    {
        result += " * ";
    }
    else
    {
        result += parseList(selectKeys);
    }

    result += " from ";
    result += tableName;

    if(conditions.size() > 0)
    {
        result += " where ";
        result += parseMap(conditions);
    }
    return result;
}

RDelete::RDelete(const QString tableName):
    tableName(tableName)
{

}

void RDelete::addCondition(const QString key, QVariant value)
{
    conditions.insert(key,value);
}

QString RDelete::sql()
{
    QString result = " delete from ";

    result += tableName;

    result += " from ";
    result += tableName;

    if(conditions.size() > 0)
    {
        result += " where ";
        result += parseMap(conditions);
    }
    return result;
}
