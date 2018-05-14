#include "filereceiveproctask.h"

#include "Network/netglobal.h"
#include "global.h"

#include "jsonkey.h"
#include "rsingleton.h"
#include "dataprocess.h"

#include <QDebug>

FileReceiveProcTask::FileReceiveProcTask(QObject *parent):
    RTask(parent)
{

}

FileReceiveProcTask::~FileReceiveProcTask()
{
    stopMe();
    wait();
}

void FileReceiveProcTask::startMe()
{
    RTask::startMe();

    runningFlag = true;

    if(!isRunning())
    {
        start();
    }
    else
    {
        G_FileRecvCondition.wakeOne();
    }
}

void FileReceiveProcTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    G_FileRecvCondition.wakeOne();
}

void FileReceiveProcTask::run()
{
    while(runningFlag)
    {
        while(runningFlag && G_FileRecvBuffs.isEmpty())
        {
            G_FileRecvMutex.lock();
            G_FileRecvCondition.wait(&G_FileRecvMutex);
            G_FileRecvMutex.unlock();
        }

        if(runningFlag && G_FileRecvBuffs.size() > 0)
        {
            G_FileRecvMutex.lock();
            QByteArray array = G_FileRecvBuffs.dequeue();
            G_FileRecvMutex.unlock();

            if(array.size() > 0)
            {
                validateRecvData(array);
            }
        }
    }
}

/*!
 * @brief 解析文件信息
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void FileReceiveProcTask::validateRecvData(const QByteArray &data)
{
    RBuffer buffer(data);
    int type;
    if(!buffer.read(type))
        return;
    if((int)type == MSG_FILE)
    {
        int msgCommand;
        if(!buffer.read(msgCommand))
            return;
        handleFileMsg((MsgCommand)msgCommand,buffer);
    }
}

void FileReceiveProcTask::handleFileMsg(MsgCommand commandType, RBuffer &obj)
{
    switch(commandType)
    {
        case MSG_FILE_CONTROL:
            RSingleton<DataProcess>::instance()->proFileControl(obj);
            break;

        case MSG_FILE_REQUEST:
            RSingleton<DataProcess>::instance()->proFileRequest(obj);
            break;

        case MSG_FILE_DATA:
            RSingleton<DataProcess>::instance()->proFileData(obj);
            break;

        default:
            break;
    }
}
