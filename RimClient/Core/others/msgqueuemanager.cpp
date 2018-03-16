#include "msgqueuemanager.h"

#include <QMutexLocker>
#include <QDebug>

MsgQueueManager::MsgQueueManager()
{

}

void MsgQueueManager::enqueue(TextRequest * request)
{
    QMutexLocker locker(&msgMutex);

    MsgDesc * desc = new MsgDesc;
    desc->request = request;

    msgMap.insert(request->textId,desc);

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<msgMap.size();
}

TextRequest *MsgQueueManager::dequeue(QString textId)
{
    QMutexLocker locker(&msgMutex);
    if(msgMap.contains(textId))
    {
        TextRequest * msgDesc = msgMap.value(textId)->request;
        msgMap.remove(textId);
        return msgDesc;
    }
    return NULL;
}
