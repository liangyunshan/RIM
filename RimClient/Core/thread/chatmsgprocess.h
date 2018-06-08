/*!
 *  @brief     聊天信息存取操作
 *  @details   封装了对于群聊、单聊信息的存储与查询操作
 *  @file      chatmsgprocess.h
 *  @author    LYS
 *  @version   1.0
 *  @date      2018.05.08
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef CHATMSGPROCESS_H
#define CHATMSGPROCESS_H

#include <QQueue>
#include <QWaitCondition>

#include "sql/database.h"
#include "datastruct.h"
#include "Network/rtask.h"

class ChatMsgProcess : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    enum taskType
    {
        C2C_TASK,   //单聊任务
        GROUP_TASK  //群聊任务
    };
    enum actionType
    {
        SAVE_MSG,   //存储消息
        QUERY_MSG   //查询消息
    };
    struct TaskQueue{
        TaskQueue():actType(SAVE_MSG),start(0),count(1){

        }
        taskType tskType;   //任务类型
        actionType actType; //操作类型
        ChatInfoUnit msg;   //消息数据（操作类型为存储消息时有效）
        uint start;         //起始查询位置（操作类型为查询消息时有效）
        uint count;         //查询条数（操作类型为查询消息时有效）
    };
    explicit ChatMsgProcess(QObject *obj = Q_NULLPTR);
    ~ChatMsgProcess();

    void startMe();
    void stopMe();

    void appendC2CStoreTask(ChatInfoUnit &msgUnit);
    void appendC2CQueryTask(QString otherID, uint begin, uint count);

    void appendGroupStoreTask(ChatInfoUnit &msgUnit);
    void appendGoupQueryTask(QString groupID, uint begin, uint count);

protected:
    void run();
    bool runningFlag;

signals:
    void C2CResultReady(ChatInfoUnitList &);
    void GroupResultReady(ChatInfoUnitList &);

private:
    bool saveC2CTaskMsg(ChatInfoUnit &msgUnit);
    bool queryC2CTaskMsg(QString otherID,uint start, uint count);

    bool saveGroupTaskMsg(ChatInfoUnit &msgUnit);
    bool queryGroupTaskMsg(QString groupID,uint start, uint count);

private:
    QQueue<TaskQueue> m_TaskQueue;
    QWaitCondition runWaitCondition;
    QMutex m_Pause;
    ChatInfoUnitList chatMsgs;
};

#endif // CHATMSGPROCESS_H
