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
#include "../protocol/datastruct.h"
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
        SAVE_MSG,       //存储消息
        QUERY_MSG,      //查询消息
        UPDATE_MSG,     //更新消息
        QUERY_MOREMSG,  //查询更多消息
        QUERY_ALLHIS,   //全部类型历史记录查询
        QUERY_IMGHIS,   //图片类型历史记录查询
        QUERY_FILEHIS   //文件类型历史记录查询
    };
    struct TaskQueue{
        TaskQueue():actType(SAVE_MSG),start(0),count(1){

        }
        taskType tskType;   //任务类型
        actionType actType; //操作类型
        QString otherID;    //联系人ID
        ChatInfoUnit msg;   //消息数据（操作类型为存储消息时有效）
        uint start;         //起始查询位置（操作类型为查询消息时有效）
        uint count;         //查询条数（操作类型为查询消息时有效）
    };
    explicit ChatMsgProcess(QObject *obj = Q_NULLPTR);
    ~ChatMsgProcess();

    void startMe();
    void stopMe();

    void appendC2CStoreTask(QString otherID, ChatInfoUnit &msgUnit);
    void appendC2CQueryTask(QString otherID, uint begin, uint count);
    void appendC2CUpdateMsgStatusTask(QString otherID, uint serialNo);

    void appendGroupStoreTask(ChatInfoUnit &msgUnit);
    void appendGoupQueryTask(QString groupID, uint begin, uint count);

    void appendC2CMoreQueryTask(QString otherID, uint begin, uint count);
    void appendC2CAllHistoryQueryTask(QString otherID, uint begin, uint count);

protected:
    void run();
    bool runningFlag;

signals:
    void C2CResultReady(ChatInfoUnitList,QString);
    void C2CMsgStatusChanged(ushort otherId,ushort serialNo);
    void GroupResultReady(ChatInfoUnitList,QString);
    void C2CMoreResultReady(ChatInfoUnitList,QString);
    void C2CHistoryResultReady(ChatInfoUnitList,QString);

private:
    bool saveC2CTaskMsg(QString otherID,ChatInfoUnit &msgUnit);
    bool queryC2CTaskMsg(QString otherID,uint start, uint count);
    bool updateC2CTaskMsgStatus(QString otherID, ushort serialNo);
    bool queryC2CTaskMoreMsg(QString otherID,uint start, uint count);
    bool queryC2CTaskAllHistory(QString otherID,uint start, uint count);
    bool queryC2CTaskImgHistory(QString otherID,uint start, uint count);
    bool queryC2CTaskFileHistory(QString otherID,uint start, uint count);

    bool saveGroupTaskMsg(ChatInfoUnit &msgUnit);
    bool queryGroupTaskMsg(QString groupID,uint start, uint count);
    bool queryGroupTaskMoreMsg(QString groupID,uint start, uint count);
    bool queryGroupTaskAllHistory(QString otherID,uint start, uint count);

private:
    QQueue<TaskQueue> m_TaskQueue;
    QWaitCondition runWaitCondition;
    QMutex m_Pause;
    ChatInfoUnitList chatMsgs;
    ChatInfoUnitList allHistory;
};

#endif // CHATMSGPROCESS_H
