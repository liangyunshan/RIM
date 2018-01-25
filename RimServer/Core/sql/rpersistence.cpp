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


Criteria::Criteria()
{

}

Criteria & Criteria::add(Restrictions rest)
{
    restricitinons.insert(rest,CADD);
    return *this;

}

Criteria & Criteria::orr(Restrictions rest)
{
    restricitinons.insert(rest,COR);
    return *this;
}


QString Criteria::toSql()
{
    QString result;

    QList<Restrictions> ctypes = restricitinons.keys();

    for(int i = 0; i < ctypes.size(); i++)
    {
        if(i > 0)
        {
            switch(restricitinons.value(ctypes.at(i)))
            {
                case CADD: result += (" and ");break;
                case COR: result += (" or ");break;
                default:break;
            }
        }
        result += ctypes.at(i).toSql();
    }

    qDebug()<<result;

    return result;
}

Restrictions::Restrictions(QString name, QVariant value, OperateType type):
    name(name),value(value),operation(type)
{

}

Restrictions Restrictions::eq(QString name, QVariant value)
{
    return Restrictions(name,value,OperateType::EQ);
}

Restrictions Restrictions::gt(QString name, QVariant value)
{
    return Restrictions(name,value,OperateType::GT);
}

Restrictions Restrictions::ge(QString name, QVariant value)
{
    return Restrictions(name,value,OperateType::GE);
}

Restrictions Restrictions::lt(QString name, QVariant value)
{
    return Restrictions(name,value,OperateType::LT);
}

Restrictions Restrictions::le(QString name, QVariant value)
{
    return Restrictions(name,value,OperateType::LE);
}

Restrictions Restrictions::ne(QString name, QVariant value)
{
    return Restrictions(name,value,OperateType::NE);
}

Restrictions Restrictions::like(QString name, QVariant value)
{
    return Restrictions(name,value,OperateType::LIKE);
}

Restrictions Restrictions::in(QString name, QVariant value)
{
    return Restrictions(name,value,OperateType::IN);
}

bool Restrictions::operator<(const Restrictions & src)const
{
    if(&src == this)
    {
        return false;
    }

    if(src.name == this->name && src.value == this->value)
    {
        return false;
    }

    return true;
}

QString Restrictions::toSql() const
{
    QString result;

    result +=(" " + name + " ");

    switch(operation)
    {
        case EQ: result += " = ";break;
        case GT: result += " > ";break;
        case GE: result += " >= ";break;
        case LT: result += " < ";break;
        case LE: result += " <= ";break;
        case NE: result += " != ";break;
        case LIKE: result += " like ";break;
        case IN: result += " in ";break;
        default:break;
    }

    switch(value.type())
    {
         case QVariant::Date:
         case QVariant::DateTime:
         case QVariant::String:
                                 result += (" '" + value.toString() + "' ");
                                 break;
         default:
                                 result += value.toString();
                                 break;
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

QString RUpdate::sql()
{
    QString result = " update ";
    result += tableName;
    result += " set ";

    result += parseMap(maps);

    if(ctia.size() > 0)
    {
        result += " where ";
        result += ctia.toSql();
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

    if(ctia.size() > 0)
    {
        result += " where ";
        result += ctia.toSql();
    }
    return result;
}

RDelete::RDelete(const QString tableName):
    tableName(tableName)
{

}

QString RDelete::sql()
{
    QString result = " delete from ";

    result += tableName;

    result += " from ";
    result += tableName;

    if(ctia.size() > 0)
    {
        result += " where ";
        result += ctia.toSql();
    }
    return result;
}
