#include "chatmsgprocess.h"

#include "../sql/database.h"
#include "../sql/rpersistence.h"
#include "../sql/datatable.h"
#include "../global.h"
#include "../user/user.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QVariant>
#include <QDebug>

ChatMsgProcess::ChatMsgProcess(QObject *obj):
    RTask(obj)
{

}

ChatMsgProcess::~ChatMsgProcess()
{
    stopMe();
    wait();
}

void ChatMsgProcess::startMe()
{
    RTask::startMe();
    runningFlag = true;

    if(!isRunning())
    {
        start();
    }
    else
    {
        runWaitCondition.wakeOne();
    }
}

void ChatMsgProcess::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    runWaitCondition.wakeOne();
}

/*!
 * @brief ChatMsgProcess::appendC2CStoreTask 加入存储单聊消息任务到任务队列中
 * @param[in] targetID 联系人账户id
 * @param[in] msgUnit 需要存储的消息数据
 */
void ChatMsgProcess::appendC2CStoreTask(QString otherID,ChatInfoUnit &msgUnit)
{
    TaskQueue t_newStoreTask;
    t_newStoreTask.tskType = C2C_TASK;
    t_newStoreTask.actType = SAVE_MSG;
    t_newStoreTask.otherID = otherID;
    t_newStoreTask.msg = msgUnit;
    t_newStoreTask.count = 0;
    m_TaskQueue.enqueue(t_newStoreTask);

    startMe();
}

/*!
 * @brief ChatMsgProcess::appendC2CQueryTask 加入查询单聊消息任务到任务队列中
 * @param[in] targetID 联系人账号id
 * @param[in] start 需要查询的消息的起始位置（最小值为0）
 * @param[in] count 需要查询的消息条数
 */
void ChatMsgProcess::appendC2CQueryTask(QString otherID, uint begin, uint count)
{
    TaskQueue t_newQueryTask;
    ChatInfoUnit t_unit;
    t_unit.accountId = G_User->BaseInfo().accountId;
    t_newQueryTask.otherID = otherID;
    t_newQueryTask.tskType = C2C_TASK;
    t_newQueryTask.actType = QUERY_MSG;
    t_newQueryTask.msg = t_unit;
    t_newQueryTask.start = begin;
    t_newQueryTask.count = count;
    m_TaskQueue.enqueue(t_newQueryTask);

    startMe();
}

/*!
 * @brief 加入查看更多消息任务到任务队列中
 * @param otherID 联系人账号id
 * @param begin 需要查询的消息的起始位置（最小值为0）
 * @param count 需要查询的消息条数
 */
void ChatMsgProcess::appendC2CMoreQueryTask(QString otherID, uint begin, uint count)
{
    TaskQueue t_newQueryTask;
    ChatInfoUnit t_unit;
    t_unit.accountId = G_User->BaseInfo().accountId;
    t_newQueryTask.otherID = otherID;
    t_newQueryTask.tskType = C2C_TASK;
    t_newQueryTask.actType = QUERY_MOREMSG;
    t_newQueryTask.msg = t_unit;
    t_newQueryTask.start = begin;
    t_newQueryTask.count = count;
    m_TaskQueue.enqueue(t_newQueryTask);

    startMe();
}

/*!
<<<<<<< HEAD
 * @brief 更新单聊界面的信息的已读和未读状态
 * @param accountid 对方ID
 * @param serialNo 信息流水号
 */
void ChatMsgProcess::appendC2CUpdateMsgStatusTask(QString otherID, uint serialNo)
{
    TaskQueue t_newQueryTask;
    ChatInfoUnit t_unit;
    t_unit.accountId = G_User->BaseInfo().accountId;
    t_unit.serialNo = serialNo;
    t_newQueryTask.otherID = otherID;
    t_newQueryTask.tskType = C2C_TASK;
    t_newQueryTask.actType = UPDATE_MSG;
    t_newQueryTask.msg = t_unit;
    m_TaskQueue.enqueue(t_newQueryTask);

    startMe();
}

/*!
 * @brief ChatMsgProcess::appendGroupStoreTask 加入存储群聊消息任务到任务队列中
 * @param targetID 群账号id
 * @param msgUnit 需要存储的群聊消息数据
 */
void ChatMsgProcess::appendGroupStoreTask(ChatInfoUnit &msgUnit)
{
    TaskQueue t_newStoreTask;
    t_newStoreTask.tskType = GROUP_TASK;
    t_newStoreTask.actType = SAVE_MSG;
    t_newStoreTask.msg = msgUnit;
    t_newStoreTask.count = 0;
    m_TaskQueue.enqueue(t_newStoreTask);

    startMe();
}

/*!
 * @brief ChatMsgProcess::appendGoupQueryTask 加入查询群聊消息任务到任务队列中
 * @param targetID 群账户id
 * @param start 需要查询的消息的起始位置（最小值为0）
 * @param count 需要查询的消息条数
 */
void ChatMsgProcess::appendGoupQueryTask(QString groupID, uint begin, uint count)
{
    TaskQueue t_newQueryTask;
    ChatInfoUnit t_unit;
    t_unit.accountId = groupID;
    t_newQueryTask.tskType = GROUP_TASK;
    t_newQueryTask.actType = QUERY_MSG;
    t_newQueryTask.msg = t_unit;
    t_newQueryTask.start = begin;
    t_newQueryTask.count = count;
    m_TaskQueue.enqueue(t_newQueryTask);

    startMe();
}

void ChatMsgProcess::run()
{
    while (runningFlag)
    {
        while(runningFlag && m_TaskQueue.isEmpty())
        {
            m_Pause.lock();
            runWaitCondition.wait(&m_Pause);
            m_Pause.unlock();
        }
        if(runningFlag && m_TaskQueue.size() > 0)
        {
            m_Pause.lock();
            TaskQueue m_task = m_TaskQueue.dequeue();
            m_Pause.unlock();

            switch (m_task.actType) {
            case SAVE_MSG:
                if(m_task.tskType == C2C_TASK)
                {
                    saveC2CTaskMsg(m_task.otherID,m_task.msg);
                }
                else
                {
                    saveGroupTaskMsg(m_task.msg);
                }
                break;
            case QUERY_MSG:
                if(m_task.tskType == C2C_TASK)
                {
                    queryC2CTaskMsg(m_task.otherID,m_task.start,m_task.count);
                }
                else
                {
                    queryGroupTaskMsg(m_task.msg.accountId,m_task.start,m_task.count);
                }
                break;
            case UPDATE_MSG:
                {
                    if(m_task.tskType == C2C_TASK)
                    {
                        updateC2CTaskMsgStatus(m_task.otherID,m_task.msg.serialNo);
                    }
                }
                break;
            case QUERY_MOREMSG:
                if(m_task.tskType == C2C_TASK)
                {
                    queryC2CTaskMoreMsg(m_task.otherID,m_task.start,m_task.count);
                }
                else
                {
                    queryGroupTaskMoreMsg(m_task.msg.accountId,m_task.start,m_task.count);
                }
                break;
            default:
                break;
            }
        }
    }
}

/*!
 * @brief ChatMsgProcess::saveC2CTaskMsg 将任务队列中目标任务的单聊信息存储到数据库
 * @param[in] msgUnit 任务中的单聊消息数据
 * @return 保存单聊信息操作结果
 */
bool ChatMsgProcess::saveC2CTaskMsg(QString otherID, ChatInfoUnit &msgUnit)
{
    DataTable::RChatRecord rcr;
    rcr.initTable("rchatrecord_"+otherID);
    RPersistence rps(rcr.table);
    rps.insert({{rcr.type,msgUnit.contentType}
               ,{rcr.accountId,msgUnit.accountId}
               ,{rcr.nickName,msgUnit.nickName}
               ,{rcr.time,msgUnit.dtime}
               ,{rcr.dateTime,msgUnit.dateTime}
               ,{rcr.serialNo,msgUnit.serialNo}
               ,{rcr.status,MSG_NOTREAD}
               ,{rcr.data,msgUnit.contents}
               });

    QSqlQuery query(G_User->database()->sqlDatabase());
    if(query.exec(rps.sql()))
    {
        return true;
    }

    return false;
}

/*!
 * @brief ChatMsgProcess::queryC2CTaskMsg 查询任务队列中任务的既定数目的单聊消息
 * @param[in] count 任务中查询单聊消息的数目
 * @return 查询单聊信息操作结果
 */
bool ChatMsgProcess::queryC2CTaskMsg(QString otherID,uint start,uint count)
{
    Q_UNUSED(start);
    chatMsgs.clear();

    DataTable::RChatRecord rcr;
    rcr.initTable("rchatrecord_"+otherID);
    RSelect rst(rcr.table);
    rst.select(rcr.table,{{rcr.id},
                          {rcr.type},
                          {rcr.accountId},
                          {rcr.nickName},
                          {rcr.time},
                          {rcr.dateTime},
                          {rcr.serialNo},
                          {rcr.status},
                          {rcr.data}})
            .createCriteria();
    rst.orderBy(rcr.table,rcr.id,SuperCondition::DESC);
    rst.limit(0,count);

    QSqlQuery query(G_User->database()->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        while(query.next())
        {
            ChatInfoUnit unitMsg;
            unitMsg.id = query.value(rcr.id).toString();
            unitMsg.contentType = static_cast<MsgCommand>(query.value(rcr.type).toInt());
            unitMsg.accountId = query.value(rcr.accountId).toString();
            unitMsg.nickName = query.value(rcr.nickName).toString();
            unitMsg.dtime = query.value(rcr.time).toLongLong();
            unitMsg.dateTime = query.value(rcr.dateTime).toString();
            unitMsg.serialNo = query.value(rcr.serialNo).toInt();
            unitMsg.contents = query.value(rcr.data).toString();
            chatMsgs.prepend(unitMsg);
        }
        if(chatMsgs.size())
            emit C2CResultReady(chatMsgs);
        return true;
    }

    return false;
}

bool ChatMsgProcess::updateC2CTaskMsgStatus(QString otherID, ushort serialNo)
{
    DataTable::RChatRecord rcr;
    rcr.initTable("rchatrecord_"+otherID);
    RUpdate rpd(rcr.table);
    rpd.update(rcr.table,{{rcr.status,MSG_READYREAD}})
            .enableAlias(false)
            .createCriteria()
            .add(Restrictions::eq(rcr.table,rcr.accountId,otherID))
            .add(Restrictions::eq(rcr.table,rcr.serialNo,serialNo));

    QSqlQuery query(G_User->database()->sqlDatabase());
    if(query.exec(rpd.sql()))
    {
        emit C2CMsgStatusChanged(otherID.toUShort(),serialNo);
        return true;
    }

    return false;
}

/*!
 * @brief 查询任务队列中查看更多消息任务的既定数目的单聊消息
 * @param otherID 联系人账号id
 * @param start 需要查询的消息的起始位置（最小值为0）
 * @param count 需要查询的消息条数
 * @return 查看更多消息操作结果
 */
bool ChatMsgProcess::queryC2CTaskMoreMsg(QString otherID, uint start, uint count)
{
    chatMsgs.clear();

    DataTable::RChatRecord rcr;
    rcr.initTable("rchatrecord_"+otherID);
    RSelect rst(rcr.table);
    rst.select(rcr.table,{{rcr.id},
                          {rcr.type},
                          {rcr.accountId},
                          {rcr.nickName},
                          {rcr.time},
                          {rcr.dateTime},
                          {rcr.serialNo},
                          {rcr.data}})
            .createCriteria();
    rst.orderBy(rcr.table,rcr.id,SuperCondition::DESC);
    rst.limit(start,count);

    QSqlQuery query(G_User->database()->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        while(query.next())
        {
            ChatInfoUnit unitMsg;
            unitMsg.id = query.value(rcr.id).toString();
            unitMsg.contentType = static_cast<MsgCommand>(query.value(rcr.type).toInt());
            unitMsg.accountId = query.value(rcr.accountId).toString();
            unitMsg.nickName = query.value(rcr.nickName).toString();
            unitMsg.dtime = query.value(rcr.time).toLongLong();
            unitMsg.dateTime = query.value(rcr.dateTime).toString();
            unitMsg.serialNo = query.value(rcr.serialNo).toInt();
            unitMsg.contents = query.value(rcr.data).toString();
            chatMsgs.append(unitMsg);
        }
        if(chatMsgs.size())
            emit C2CMoreResultReady(chatMsgs);
        return true;
    }

    return false;
}

/*!
 * @brief ChatMsgProcess::saveGroupTaskMsg 将任务队列中目标任务的群聊信息存储到数据库
 * @param msgUnit 任务中的群聊消息数据
 * @return 保存群聊信息操作结果
 */
bool ChatMsgProcess::saveGroupTaskMsg(ChatInfoUnit &msgUnit)
{
    //TODO LYS-实现保存群聊信息到群聊信息记录表中
    Q_UNUSED(msgUnit);

    return false;
}

/*!
 * @brief ChatMsgProcess::queryGroupTaskMsg 查询任务队列中任务的既定数目的群聊消息
 * @param groupID 任务中查询群聊消息的数目
 * @param start
 * @param count
 * @return 查询群聊信息操作结果
 */
bool ChatMsgProcess::queryGroupTaskMsg(QString groupID, uint start, uint count)
{
    //TODO LYS-实现查询群聊信息记录表中群聊信息
    Q_UNUSED(groupID);
    Q_UNUSED(start);
    Q_UNUSED(count);

    return false;
}

/*!
 * @brief ChatMsgProcess::queryGroupTaskMoreMsg
 * @param groupID
 * @param start
 * @param count
 * @return
 */
bool ChatMsgProcess::queryGroupTaskMoreMsg(QString groupID, uint start, uint count)
{
    //TODO LYS-20180718 实现查询群聊信息记录表中更多群聊信息
    Q_UNUSED(groupID);
    Q_UNUSED(start);
    Q_UNUSED(count);

    return false;
}
