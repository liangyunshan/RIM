#include "sqlprocess.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <QDebug>

#include "database.h"
#include "rpersistence.h"
#include "datatable.h"


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
    m_QueryRow = DefaultQueryRow;
    p_SQLProcess= this;
}

SQLProcess::~SQLProcess()
{
    if(p_SQLProcess)
    {
        delete p_SQLProcess;
    }
    p_SQLProcess = NULL;
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

//    if(!query.exec(DataTable::SQLChatRecord))
//        return false;

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
 * @return 添加结果
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
