/*!
 *  @brief     消息队列管理
 *  @details   保存发送的消息
 *  @file      msgqueuemanager.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.02.26
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGQUEUEMANAGER_H
#define MSGQUEUEMANAGER_H

#include <QMap>
#include <QMutex>

#include "protocoldata.h"
using namespace ProtocolType;

class MsgQueueManager
{
public:
    MsgQueueManager();

    void enqueue(TextRequest * request);
    TextRequest *dequeue(QString textId);

private:
    struct MsgDesc
    {
        MsgDesc()
        {
            resendTime = 0;
        }
        TextRequest * request;          /*!< 发送信息 */
        int resendTime;                 /*!< 重发次数，大于3次则直接提示发送失败，不记录至数据库 */
    };

    QMap<QString,MsgDesc *> msgMap;
    QMutex msgMutex;
};

#endif // MSGQUEUEMANAGER_H
