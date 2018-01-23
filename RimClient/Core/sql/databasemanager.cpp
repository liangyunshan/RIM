#include "databasemanager.h"

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "Util/rlog.h"
#include "Util/rutil.h"

DatabaseManager *p_This = NULL;

DatabaseManager::DatabaseManager():
    m_hostName(""),m_dbName(""),m_dbUser(""),m_dbPass(""),m_port(0),
    m_dbType("")
{
    p_DB = NULL;
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

//创建一个数据库连接
Database *DatabaseManager::newDatabase(QString connectionName)
{
    Database * db = new Database(m_dbType,connectionName);

    db->setHostName(m_hostName);
    db->setDatabaseName(m_dbName);
    db->setUserName(m_dbUser);
    db->setPassword(m_dbPass);

    db->open();

    p_DB = db;
    createTablebUserList();
    return db;
}

//创建曾有过聊天记录的好友列表
bool DatabaseManager::createTablebUserList()
{
    if(!p_DB)
    {
        return false;
    }
    QSqlQuery query(p_DB->sqlDatabase());
    query.prepare(QString("create table %1(%2 int primary key,%3 varchar )")
                  .arg(TextUnit::_Sql_Table_01_Name_)
                  .arg(TextUnit::_Sql_UserList_UserId_)
                  .arg(TextUnit::_Sql_UserList_UserName_));
    bool ret = query.exec();

    return ret;
}

//查询一个用户记录是否存在
bool DatabaseManager::queryUser(int tgtUserId)
{
    if(!p_DB)
    {
        return false;
    }

    QSqlQuery query(p_DB->sqlDatabase());

    query.prepare(QString("select %1 from %2")
                  .arg(tgtUserId)
                  .arg(TextUnit::_Sql_Table_01_Name_));

    bool ret = query.exec();
    if(!ret)
    {
        return false;
    }
    int count = 0;
    while (query.next())
    {
        count++;
    }

    if(count>0)
    {
        return true;
    }
    return false;
}

//插入一个用户记录
bool DatabaseManager::insertTgtUser(int tgtUserId, QString name)
{
    if(!p_DB)
    {
        return false;
    }

    QSqlQuery query(p_DB->sqlDatabase());

    query.prepare(QString("insert into %1 values(?, ?) ")
                  .arg(TextUnit::_Sql_Table_01_Name_));
    query.bindValue(0,tgtUserId);
    query.bindValue(1,name);

    bool ret = query.exec();
    if(!ret)
    {
        return false;
    }

    return true;
}

//创建与好友的聊天记录表
bool DatabaseManager::createTableUser_id(int tgtUserId)
{
    if(!p_DB)
    {
        return false;
    }
    QSqlQuery query(p_DB->sqlDatabase());
    QString table = QString("%1 int,%2 varchar,%3 varchar,%4 varchar,%5 varchar,%6 varchar")
                    .arg(TextUnit::_Sql_User_UserId_)
                    .arg(TextUnit::_Sql_User_UserName_)
                    .arg(TextUnit::_Sql_User_Userhead_)
                    .arg(TextUnit::_Sql_User_RecordTime_)
                    .arg(TextUnit::_Sql_User_RecordContents_)
                    .arg(TextUnit::_Sql_User_RecordTxt_);
    query.prepare(QString("create table %1%2(%3)")
                  .arg(TextUnit::_Sql_Table_02_Name_)
                  .arg(tgtUserId)
                  .arg(table));

    bool ret = query.exec();

    return ret;
}

//插入一条聊天记录
bool DatabaseManager::insertTableUserChatInfo(TextUnit::ChatInfoUnit unit)
{
    if(!queryUser(unit.user.id))
    {
        insertTgtUser(unit.user.id,unit.user.name);
        createTableUser_id(unit.user.id);
    }

    QSqlQuery query(p_DB->sqlDatabase());

    query.prepare(QString("insert into %1%2 values(?,?,?,?,?,?) ")
                  .arg(TextUnit::_Sql_Table_02_Name_)
                  .arg(unit.user.id));
    query.bindValue(0,unit.user.id);
    query.bindValue(1,unit.user.name);
    query.bindValue(2,unit.user.head);
    query.bindValue(3,unit.time);
    query.bindValue(4,unit.contents);
    query.bindValue(5,unit.contents);

    bool ret = query.exec();
    if(!ret)
    {
        return false;
    }

    return true;
}

//查询历史记录
//userid表示需要查询谁的聊天记录,群号或好友号
//currRow从位置1开始，对应总的数据记录数，为0时表示没有数据
//rows表示每次需要查询的数据总数
QString DatabaseManager::querryRecords(int userid, int currRow, int rows)
{
    if(!p_DB)
    {
        return "";
    }
    int StartRow = 0;
    if(rows<=0)
    {
        rows = m_QueryRow;
    }
    if(currRow<0)
    {
        currRow = 0;
    }

    if((currRow-rows)<0)
    {
        StartRow = 0;
    }
    else
    {
        StartRow = currRow-rows;
    }

    QString cmd = QString("select * from %1%2 limit %3,%4 ")
                  .arg(TextUnit::_Sql_Table_02_Name_)
                  .arg(userid)
                  .arg(StartRow)
                  .arg(rows);
    return cmd;
}

QStringList DatabaseManager::availableDrivers()
{
    return QSqlDatabase::drivers();
}
