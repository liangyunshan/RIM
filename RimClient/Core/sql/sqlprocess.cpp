#include "sqlprocess.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <QDebug>

#include "database.h"
#include "rpersistence.h"
#include "datatable.h"

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
//        createTableUser_id(db,user_insert.id);
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
//        ret = createTableUser_id(db,user_insert.id);
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

/*!
 * @brief 创建本地数据库表
 * @details 用户在登陆成功后，即执行此操作，目前分别创建以下表: @n
 *          [1]历史会话信息表：RHistoryChat用于客户端历史聊天页面显示;
 *          [2]聊天清单表：RChatList用于记录用户和已聊天好友的信息，便于分类归纳；
 *          [3]聊天记录表：RChatRecord用于记录登陆用户和好友之间的聊天信息；
 * @param[in] db 数据库连接
 * @return 是否创建成功
 */
bool SQLProcess::createTableIfNotExists(Database * db)
{
    Q_ASSERT_X(db != nullptr,"","Database is nullptr!");

    QSqlQuery query(db->sqlDatabase());

    if(!query.exec(DataTable::SQLHistoryChat))
        return false;

    if(!query.exec(DataTable::SQLChatList))
        return false;

    if(!query.exec(DataTable::SQLChatRecord))
        return false;

    return true;
}

/*!
 * @brief 从本地库中加载历史会话信息列表
 * @param[in] db 数据库连接
 * @param[in] list 数据容器
 * @return 是否加载成功
 */
bool SQLProcess::loadChatHistoryChat(Database *db, QList<HistoryChatRecord> &list)
{
    DataTable::RHistoryChat rhc;
    RSelect rst(rhc.table);
    rst.orderBy(rhc.table,rhc.dtime,SuperCondition::DESC);

    QSqlQuery query(db->sqlDatabase());

    if(query.exec(rst.sql())){
        while(query.next()){
            HistoryChatRecord record;
            record.id = query.value(rhc.id).toString();
            record.type = static_cast<ChatMessageType>(query.value(rhc.type).toInt());
            record.accountId = query.value(rhc.accountId).toString();
            record.nickName = query.value(rhc.nickName).toString();
            record.dtime = query.value(rhc.dtime).toLongLong();
            record.lastRecord = query.value(rhc.lastRecord).toString();
            record.isTop = query.value(rhc.isTop).toBool();
            record.systemIon = query.value(rhc.systemIon).toBool();
            record.iconId = query.value(rhc.iconId).toString();
            list.push_back(record);
        }
        return true;
    }

    return false;
}

/*!
 * @brief 创建一条对话记录
 * @param[in] db 数据库连接
 * @param[in] record 连接信息
 * @return 创建结果
 */
bool SQLProcess::addOneHistoryRecord(Database *db, const HistoryChatRecord &record)
{
    if(containHistoryRecord(db,record.accountId))
        return true;

    DataTable::RHistoryChat rhc;
    RPersistence rps(rhc.table);
    rps.insert({{rhc.type,record.type}
               ,{rhc.accountId,record.accountId}
               ,{rhc.nickName,record.nickName}
               ,{rhc.dtime,record.dtime}
               ,{rhc.lastRecord,record.lastRecord}
               ,{rhc.isTop,(int)record.isTop}
               ,{rhc.systemIon,(int)record.systemIon}
               ,{rhc.iconId,record.iconId}});

    QSqlQuery query(db->sqlDatabase());

    if(query.exec(rps.sql()))
        return true;

    return false;
}

bool SQLProcess::updateOneHistoryRecord(Database *db, const HistoryChatRecord &record)
{
    if(!containHistoryRecord(db,record.accountId))
        return addOneHistoryRecord(db,record);

    DataTable::RHistoryChat rhc;
    RUpdate rpd(rhc.table);
    rpd.update(rhc.table,{{rhc.nickName,record.nickName}
                          ,{rhc.dtime,record.dtime}
                          ,{rhc.isTop,(int)record.isTop}
                          ,{rhc.systemIon,(int)record.systemIon}
                          ,{rhc.lastRecord,record.lastRecord}
                          ,{rhc.iconId,record.iconId}})
            .enableAlias(false)
            .createCriteria()
            .add(Restrictions::eq(rhc.table,rhc.accountId,record.accountId));
//            .orr(Restrictions::eq(rhc.table,rhc.id,record.id));

    QSqlQuery query(db->sqlDatabase());

    if(query.exec(rpd.sql()))
        return true;

    return false;
}

/*!
 * @brief 是否包含指定数据记录
 * @param[in] db 数据库连接
 * @param[in] record 待测试数据记录ID
 * @return 包含结果
 */
bool SQLProcess::containHistoryRecord(Database *db, const QString recordId)
{
    DataTable::RHistoryChat rhc;
    RSelect rst(rhc.table);
    rst.createCriteria()
            .add(Restrictions::eq(rhc.table,rhc.accountId,recordId));

    QSqlQuery query(db->sqlDatabase());

    if(query.exec(rst.sql()) && query.next())
        return true;

    return false;
}

/*!
 * @brief 移除指定的历史记录
 * @param[in] db 数据库连接
 * @param[in] recordId 指定移除数据ID
 * @return 是否删除成功
 */
bool SQLProcess::removeOneHistoryRecord(Database *db, QString recordId)
{
    DataTable::RHistoryChat rhc;
    RDelete rde(rhc.table);
    rde.createCriteria()
            .add(Restrictions::eq(rhc.table,rhc.accountId,recordId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rde.sql()))
        return true;

    return false;
}

/*!
 * @brief 移除全部的历史记录
 * @param[in] db 数据库连接
 * @return 是否删除成功
 */
bool SQLProcess::removeAllHistoryRecord(Database *db)
{
    DataTable::RHistoryChat rhc;
    RDelete rde(rhc.table);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rde.sql()))
        return true;

    return false;
}

/*!
 * @brief 设置置顶状态
 * @param[in] db 数据库连接
 * @param[in] recordId 指定操作数据ID
 * @param[in] isTop 置顶状态
 * @return 是否插入成功
 */
bool SQLProcess::topHistoryRecord(Database *db, QString recordId, bool isTop)
{
    DataTable::RHistoryChat rhc;
    RUpdate rpd(rhc.table);
    rpd.update(rhc.table,{{rhc.isTop,(int)isTop}})
            .enableAlias(false)
            .createCriteria()
            .add(Restrictions::eq(rhc.table,rhc.accountId,recordId));

    QSqlQuery query(db->sqlDatabase());

    if(query.exec(rpd.sql()))
        return true;

    return false;
}
