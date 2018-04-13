#include "sqlprocess.h"

#include "database.h"
#include <QSqlQuery>
#include <QVariant>

#include <QDebug>

using namespace TextUnit;

SQLProcess *p_SQLProcess = NULL;
SQLProcess *SQLProcess::instance()
{
    if(p_SQLProcess == NULL)
    {
        new SQLProcess();
    }
    return p_SQLProcess;
}

SQLProcess::SQLProcess()
{
    m_QueryRow = TextUnit::DefaultQueryRow;
    p_SQLProcess= this;
}

SQLProcess::~SQLProcess()
{
    p_SQLProcess = NULL;
}

//查询历史记录信息
QString SQLProcess::querryRecords(int userid, int currRow, int queryRows)
{
    int StartRow = 0;
    if(queryRows<=0)
    {
        queryRows = m_QueryRow;
    }
    if(currRow<0)
    {
        currRow = 0;
    }

    if((currRow-queryRows)<0)
    {
        StartRow = 0;
        queryRows = currRow;
    }
    else
    {
        StartRow = currRow-queryRows;
    }

    QString item = QString("%1,%2,%3,%4,%5,%6,%7")
            .arg(TextUnit::_Sql_User_Rowid_)
            .arg(TextUnit::_Sql_User_UserId_)
            .arg(TextUnit::_Sql_User_UserName_)
            .arg(TextUnit::_Sql_User_Userhead_)
            .arg(TextUnit::_Sql_User_RecordTime_)
            .arg(TextUnit::_Sql_User_RecordContents_)
            .arg(TextUnit::_Sql_User_RecordTxt_);

    QString cmd_limit = QString("select %0 from %1%2 limit %3,%4 ")
                  .arg(item)
                  .arg(TextUnit::_Sql_User_TableName_)
                  .arg(userid)
                  .arg(StartRow)
                  .arg(queryRows);

    QString cmd = QString("select %0 from (%1) order by %2 desc ")
            .arg(item)
            .arg(cmd_limit)
            .arg(TextUnit::_Sql_User_Rowid_);
    return cmd;
}

//查询一个表一共有多少数据记录
int SQLProcess::queryTotleRecord(Database *db, int id)
{
    QSqlQuery query(db->sqlDatabase());

    query.prepare(QString("select %0 from %1%2 order by (%3) desc")
                    .arg(TextUnit::_Sql_User_Rowid_)
                    .arg(TextUnit::_Sql_User_TableName_)
                    .arg(id)
                    .arg(TextUnit::_Sql_User_Rowid_));
    if(query.exec())
    {
        int totleId = 0;
        if(query.next())
        {
            totleId = query.value(QString(TextUnit::_Sql_User_Rowid_)).toInt();
        }
        return totleId;
    }
    return 0;
}

bool SQLProcess::insertTableUserChatInfo(Database *db, ChatInfoUnit unit, SimpleUserInfo userInfo)
{
    UserInfo user_insert;

    if(userInfo.accountId.toInt() == 0)
    {
        user_insert = unit.user;
    }
    else
    {
        user_insert.id = userInfo.accountId.toInt();
        user_insert.name = userInfo.nickName;
        user_insert.head = userInfo.iconId;
    }

    if(!queryUser(db,user_insert.id))
    {
        insertTgtUser(db,user_insert.id,user_insert.name);
        createTableUser_id(db,user_insert.id);
    }

    QSqlQuery query(db->sqlDatabase());

    query.prepare(QString("insert into %1%2 values(?,?,?,?,?,?) ")
                  .arg(TextUnit::_Sql_User_TableName_)
                  .arg(user_insert.id));
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

bool SQLProcess::initTableUser_id(Database *db, SimpleUserInfo userInfo)
{
    UserInfo user_insert ;
    user_insert.id = userInfo.accountId.toInt();
    user_insert.name = userInfo.nickName;
    user_insert.head = userInfo.iconId;

    bool ret = true;
    if(!queryUser(db,user_insert.id))
    {
        ret = insertTgtUser(db,user_insert.id,user_insert.name);
        ret = createTableUser_id(db,user_insert.id);
    }
    return ret;
}

//查询一个用户记录是否存在
bool SQLProcess::queryUser(Database * db,int tgtUserId)
{
    if(!db)
    {
        return false;
    }
    QSqlQuery query(db->sqlDatabase());
    query.clear();
    QString cmd = QString("select %1 from %2")
                        .arg(tgtUserId)
                        .arg(TextUnit::_Sql_UserList_TableName_);
    query.prepare(cmd);
    bool ret = query.exec();
    if(!ret)
    {
        return false;
    }
    int count = 0;
    while (query.next())
    {
        count++;
        break;
    }

    if(count>0)
    {
        return true;
    }
    return false;
}

//插入一个用户记录
bool SQLProcess::insertTgtUser(Database *db, int tgtUserId, QString name)
{
    if(!db)
    {
        return false;
    }

    QSqlQuery query(db->sqlDatabase());

    query.prepare(QString("insert into %1 values(?, ?) ")
                  .arg(TextUnit::_Sql_UserList_TableName_));
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
bool SQLProcess::createTableUser_id(Database * db,int tgtUserId)
{
    if(!db)
    {
        return false;
    }
    QSqlQuery query(db->sqlDatabase());
    QString table = QString("%1 int,%2 varchar,%3 varchar,%4 varchar,%5 varchar,%6 varchar")
                    .arg(TextUnit::_Sql_User_UserId_)
                    .arg(TextUnit::_Sql_User_UserName_)
                    .arg(TextUnit::_Sql_User_Userhead_)
                    .arg(TextUnit::_Sql_User_RecordTime_)
                    .arg(TextUnit::_Sql_User_RecordContents_)
                    .arg(TextUnit::_Sql_User_RecordTxt_);
    query.prepare(QString("create table %1%2(%3)")
                  .arg(TextUnit::_Sql_User_TableName_)
                  .arg(tgtUserId)
                  .arg(table));

    bool ret = query.exec();

    return ret;
}

//创建曾有过聊天记录的好友列表
bool SQLProcess::createTablebUserList(Database * db)
{
    if(!db)
    {
        return false;
    }

    QSqlQuery query(db->sqlDatabase());
    query.prepare(QString("create table %1(%2 int primary key,%3 varchar )")
                  .arg(TextUnit::_Sql_UserList_TableName_)
                  .arg(TextUnit::_Sql_UserList_UserId_)
                  .arg(TextUnit::_Sql_UserList_UserName_));
    bool ret = query.exec();

    return ret;
}
