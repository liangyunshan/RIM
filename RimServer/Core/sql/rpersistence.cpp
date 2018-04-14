#include "rpersistence.h"

#include <QDebug>
#include <QTextStream>

const QString spacer = " ";

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
    QTextStream stream(&result,QIODevice::ReadWrite);

    QList<Restrictions> ctypes = restricitinons.keys();

    for(int i = 0; i < ctypes.size(); i++)
    {
        if(i > 0)
        {
            switch(restricitinons.value(ctypes.at(i)))
            {
                case CADD: stream<<spacer<<"AND"<<spacer;break;
                case COR: stream<<spacer<<"OR"<<spacer;break;
                default:break;
            }
        }
        stream<<ctypes.at(i).toSql();
    }
    stream.flush();

    return result;
}

QString Criteria::toSql(QMap<QString, QString> tableAlias)
{
    QString result;
    QTextStream stream(&result,QIODevice::ReadWrite);
    QList<Restrictions> ctypes = restricitinons.keys();

    for(int i = 0; i < ctypes.size(); i++)
    {
        if(i > 0)
        {
            switch(restricitinons.value(ctypes.at(i)))
            {
                case CADD: stream<<"AND"<<spacer;break;
                case COR: stream<<"OR"<<spacer;break;
                default:break;
            }
        }
        QString alias = tableAlias.value(ctypes.at(i).tableName());
        stream<<ctypes.at(i).toSql(alias);
    }
    stream.flush();

    return result;
}

Restrictions::Restrictions(QString tName ,QString key,QVariant value,OperateType type):
    name(key),value(value),operation(type),tname(tName)
{

}

Restrictions Restrictions::eq(QString tName,QString key,QVariant value)
{
    return Restrictions(tName,key,value,OperateType::EQ);
}

Restrictions Restrictions::gt(QString tName,QString key,QVariant value)
{
    return Restrictions(tName,key,value,OperateType::GT);
}

Restrictions Restrictions::ge(QString tName,QString key,QVariant value)
{
    return Restrictions(tName,key,value,OperateType::GE);
}

Restrictions Restrictions::lt(QString tName,QString key,QVariant value)
{
    return Restrictions(tName,key,value,OperateType::LT);
}

Restrictions Restrictions::le(QString tName,QString key,QVariant value)
{
    return Restrictions(tName,key,value,OperateType::LE);
}

Restrictions Restrictions::ne(QString tName,QString key,QVariant value)
{
    return Restrictions(tName,key,value,OperateType::NE);
}

Restrictions Restrictions::like(QString tName,QString key,QVariant value)
{
    return Restrictions(tName,key,value,OperateType::LIKE);
}

Restrictions Restrictions::in(QString tName,QString key,QVariant value)
{
    return Restrictions(tName,key,value,OperateType::IN);
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

QString Restrictions::toSql(QString tableAlais) const
{
    QString result;
    QTextStream stream(&result,QIODevice::ReadWrite);

    if(tableAlais.size() > 0)
        stream<<spacer<<tableAlais<<spacer<<"."<<name<<spacer;
    else
        stream<<spacer<<name<<spacer;

    switch(operation)
    {
        case EQ: stream<<spacer<<"="<<spacer;break;
        case GT: stream<<spacer<<">"<<spacer;break;
        case GE: stream<<spacer<<">="<<spacer;break;
        case LT: stream<<spacer<<"<"<<spacer;break;
        case LE: stream<<spacer<<"<="<<spacer;break;
        case NE: stream<<spacer<<"!="<<spacer;break;
        case LIKE: stream<<spacer<<"like"<<spacer;break;
        case IN: stream<<spacer<<"in"<<spacer;break;
        default:break;
    }

    switch(value.type())
    {
         case QVariant::Date:
         case QVariant::DateTime:
         case QVariant::String:
                                 stream<<spacer<<"'"<<value.toString()<<"'"<<spacer;
                                 break;
         default:
                                 stream<<value.toString();
                                 break;
    }

    stream.flush();

    return result;
}

RPersistence::RPersistence(const QString tableName):
    tableName(tableName)
{

}

RPersistence & RPersistence::insert(const QString key, QVariant value)
{
    maps.insert(key,value);
    return *this;
}

RPersistence &RPersistence::insert(std::vector<std::pair<QString, QVariant> > list)
{
    std::for_each(list.begin(),list.end(),[&](const std::pair<QString,QVariant> & pair){
        this->maps.insert(pair.first,pair.second);
    });
    return *this;
}

QString RPersistence::sql()
{
    QString result;
    QTextStream stream(&result,QIODevice::ReadWrite);
    stream<<"INSERT INTO"<<spacer;

    stream<<tableName;
    stream<<spacer<<"("<<spacer;

    QList<QString> keys = maps.keys();

    result += parseList(keys);
    stream<<spacer<<") VALUES ("<<spacer;

    for(int i = 0; i < keys.size(); i++)
    {
       QVariant val = maps.value(keys.at(i));

       switch(val.type())
       {
            case QVariant::Date:
            case QVariant::DateTime:
            case QVariant::String:
                                    stream<<spacer<<"'"<<val.toString()<<"'"<<spacer;
                                    break;
            default:
                                    stream<<val.toString();
                                    break;
       }
       if(i < keys.size() - 1)
       {
           stream<<","<<spacer;
       }
       else
       {
           stream<<spacer<<")"<<spacer;
       }
    }
    stream.flush();

    return result;
}

RUpdate::RUpdate(std::initializer_list<QString> tNames){
    QString alias("T%1");
    int index = 0;
    for(auto p:tNames){
        tableNames.insert(p,alias.arg(index++));
    }
}

RUpdate::RUpdate(const QString tName){
    QString alias("T%1");
    tableNames.insert(tName,alias.arg(0));
}

RUpdate & RUpdate::update(const QString tName,const QString key, QVariant value)
{
    updateKeys.push_back(UpdateKeys{tName,key,value});
    return *this;
}

RUpdate &RUpdate::update(const QString tName, std::vector<std::pair<QString, QVariant> > list)
{
    std::for_each(list.begin(),list.end(),[&](const std::pair<QString,QVariant> & item){
        this->updateKeys.push_back(UpdateKeys{tName,item.first,item.second});
    });
    return *this;
}

RUpdate &RUpdate::on(const QString &tName1, const QString key1, const QString tName2, const QString value2)
{
    onCondtions.push_back({tName1,key1,tName2,value2});
    return *this;
}

QString RUpdate::sql()
{
    QString result;
    QTextStream stream(&result,QIODevice::ReadWrite);
    stream<<"UPDATE"<<spacer;

    if(tableNames.size() > 1){
        for(int i = 0; i < tableNames.size(); i++){
            stream<<tableNames.keys().at(i)<<spacer<<tableNames.value(tableNames.keys().at(i));
            if(i < tableNames.size() - 1){
                stream<<spacer<<"LEFT JOIN"<<spacer;
            }
        }
        stream<<spacer<<"ON"<<spacer;
    }
    else
        stream<<tableNames.keys().at(0)<<spacer<<tableNames.value(tableNames.keys().at(0))<<spacer;

    if(onCondtions.size() > 0){
        for(int i = 0; i < onCondtions.size(); i++){
            OnContion condition = onCondtions.at(i);
            stream<<tableNames.value(condition.t1)<<"."<<condition.v1<<"="<<tableNames.value(condition.t2)<<"."<<condition.v2;
            if(i < onCondtions.size() - 1){
                stream<<spacer<<"AND"<<spacer;
            }
        }
    }

    stream<<spacer<<"SET"<<spacer;

    for(int i = 0; i < updateKeys.size(); i++)
    {
        const UpdateKeys & key = updateKeys.at(i);
        stream<<tableNames.value(key.tname)<<"."<<key.tKey;
        stream<<spacer<<"="<<spacer;

        switch(key.value.type())
        {
             case QVariant::Date:
             case QVariant::DateTime:
             case QVariant::String:
                                     stream<<spacer<<"'"<<key.value.toString()<<"'"<<spacer;
                                     break;
             default:
                                     stream<<key.value.toString();
                                     break;
        }

        if(i < updateKeys.size() - 1){
            stream<<spacer<<","<<spacer;
        }else{
            stream<<spacer;
        }
    }

    if(ctia.size() > 0)
    {
        stream<<spacer<<"WHERE"<<spacer;
        stream<<ctia.toSql(tableNames);
    }
    stream.flush();

    return result;
}

RSelect::RSelect(std::initializer_list<QString> tNames):limitStart(-1),limitCount(-1),isSetLimit(false)
{
    QString alias("T%1");
    int index = 0;
    for(auto p:tNames){
        tableNames.insert(p,alias.arg(index++));
    }
}

RSelect::RSelect(QString tName):limitStart(-1),limitCount(-1),isSetLimit(false)
{
    QString alias("T%1");
    tableNames.insert(tName,alias.arg(0));
}

RSelect& RSelect::select(const QString &tName, std::initializer_list<QString> keys)
{
    for(auto key : keys)
        selectedKeys.push_back(Keys{tName,key});
    return *this;
}

RSelect &RSelect::on(const QString &tName1, const QString key1, const QString tName2, const QString value2)
{
    onCondtions.push_back({tName1,key1,tName2,value2});
    return *this;
}

bool RSelect::limit(unsigned int start, unsigned int count)
{
    isSetLimit = false;
    if(start >= 0 && count > 0){
        limitStart = start;
        limitCount = count;
        isSetLimit = true;
    }
    return isSetLimit;
}

RSelect &RSelect::orderBy(const QString &tName, const QString key, SuperCondition::SOrder odr)
{
    sortOrders.push_back(Orders{tName,key,odr});
    return *this;
}

QString RSelect::sql()
{
    QString result;
    QTextStream stream(&result,QIODevice::ReadWrite);
    stream<<spacer<<"SELECT"<<spacer;

    if(selectedKeys.size() > 0){
        for(int i = 0;i < selectedKeys.size(); i++){
            stream<<tableNames.value(selectedKeys.at(i).tname)<<"."<<selectedKeys.at(i).tkey;
            if(i != selectedKeys.size() - 1){
                stream<<spacer<<","<<spacer;
            }
        }
    }else{
        stream<<spacer<<"*"<<spacer;
    }

    stream<<spacer<<"from"<<spacer;

    if(tableNames.size() > 1){
        for(int i = 0; i < tableNames.size(); i++){
            stream<<tableNames.keys().at(i)<<spacer<<tableNames.value(tableNames.keys().at(i));
            if(i < tableNames.size() - 1){
                stream<<spacer<<"LEFT JOIN"<<spacer;
            }
        }
        stream<<spacer<<"ON"<<spacer;
    }
    else
        stream<<tableNames.keys().at(0)<<spacer<<tableNames.value(tableNames.keys().at(0))<<spacer;

    if(onCondtions.size() > 0){
        for(int i = 0; i < onCondtions.size(); i++){
            OnContion condition = onCondtions.at(i);
            stream<<tableNames.value(condition.t1)<<"."<<condition.v1<<"="<<tableNames.value(condition.t2)<<"."<<condition.v2;
            if(i < onCondtions.size() - 1){
                stream<<spacer<<"AND"<<spacer;
            }
        }
    }

    if(ctia.size() > 0)
    {
        stream<<spacer<<"WHERE"<<spacer;
        stream<<ctia.toSql(tableNames);
    }

    if(sortOrders.size() > 0){
        stream<<spacer<<"GROUP BY"<<spacer;
        for(int i = 0; i < sortOrders.size();i++){
            stream<<tableNames.value(sortOrders.at(i).tName)<<"."<<sortOrders.at(i).tkey<<spacer<<odrToString(sortOrders.at(i).odr);
        }
    }

    if(isSetLimit){
        stream<<spacer<<"LIMIT"<<spacer<<limitStart<<spacer+","<<spacer<<limitCount;
    }

    stream.flush();

    return result;
}

RDelete::RDelete(const QString tableName):
    tableName(tableName)
{

}

QString RDelete::sql()
{
    QString result;
    QTextStream stream(&result,QIODevice::ReadWrite);
    stream<<"DELETE FROM"<<spacer;

    stream<<tableName<<spacer;

    if(ctia.size() > 0)
    {
        stream<<spacer<<"WHERE"<<spacer;
        result += ctia.toSql();
    }
    return result;
}
