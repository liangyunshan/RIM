#include "chatmsgprocess.h"

#include "../sql/database.h"
#include "../sql/rpersistence.h"
#include "../sql/datatable.h"
#include "../global.h"
#include "../user/user.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

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
void ChatMsgProcess::appendC2CStoreTask(ChatInfoUnit &msgUnit)
{
    TaskQueue t_newStoreTask;
    t_newStoreTask.tskType = C2C_TASK;
    t_newStoreTask.actType = SAVE_MSG;
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
    t_unit.accountId = otherID;
    t_newQueryTask.tskType = C2C_TASK;
    t_newQueryTask.actType = QUERY_MSG;
    t_newQueryTask.msg = t_unit;
    t_newQueryTask.start = begin;
    t_newQueryTask.count = count;
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
                    saveC2CTaskMsg(m_task.msg);
                }
                else
                {
                    saveGroupTaskMsg(m_task.msg);
                }

                break;
            case QUERY_MSG:
                if(m_task.tskType == C2C_TASK)
                {
                    queryC2CTaskMsg(m_task.msg.accountId,m_task.start,m_task.count);
                }
                else
                {
                    queryGroupTaskMsg(m_task.msg.accountId,m_task.start,m_task.count);
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
bool ChatMsgProcess::saveC2CTaskMsg(ChatInfoUnit &msgUnit)
{
    DataTable::RChatRecord rcr;
    RPersistence rps(rcr.table);
    rps.insert({{rcr.type,msgUnit.contentType}
               ,{rcr.accountId,msgUnit.accountId}
               ,{rcr.nickName,msgUnit.nickName}
               ,{rcr.time,msgUnit.dtime}
               ,{rcr.data,msgUnit.contents}
               });

    QSqlQuery query(G_User->database()->sqlDatabase());

    if(query.exec(rps.sql()))
        return true;

    return false;
}

/*!
 * @brief ChatMsgProcess::queryC2CTaskMsg 查询任务队列中任务的既定数目的单聊消息
 * @param[in] count 任务中查询单聊消息的数目
 * @return 查询单聊信息操作结果
 */
bool ChatMsgProcess::queryC2CTaskMsg(QString otherID,uint start,uint count)
{
    chatMsgs.clear();

    DataTable::RChatRecord rcr;
    RSelect rst(rcr.table);
    rst.select(rcr.table,{{rcr.id},
                          {rcr.type},
                          {rcr.accountId},
                          {rcr.nickName},
                          {rcr.time},
                          {rcr.data}})
            .createCriteria()
            .add(Restrictions::eq(rcr.table,rcr.accountId,otherID));
    rst.limit(start,count);

    QSqlQuery query(G_User->database()->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        if(query.numRowsAffected() > 0)
        {
            while(query.next())
            {
                ChatInfoUnit unitMsg;
                unitMsg.id = query.value(rcr.id).toString();
                unitMsg.contentType = static_cast<MsgCommand>(query.value(rcr.type).toInt());
                unitMsg.accountId = query.value(rcr.accountId).toString();
                unitMsg.nickName = query.value(rcr.nickName).toString();
                unitMsg.dtime = query.value(rcr.time).toLongLong();
                unitMsg.contents = query.value(rcr.data).toString();
                chatMsgs.append(unitMsg);
            }
            emit C2CResultReady(chatMsgs);
        }
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
