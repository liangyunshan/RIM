#include "msgreceiveproctask.h"

#include <QJsonDocument>

#include "network/netglobal.h"
#include "dataprocess.h"
#include "rsingleton.h"
#include "head.h"
#include "jsonkey.h"

MsgReceiveProcTask::MsgReceiveProcTask(QObject *parent):
    runningFlag(false),
    RTask(parent)
{

}

MsgReceiveProcTask::~MsgReceiveProcTask()
{
    wait();
}

void MsgReceiveProcTask::startMe()
{
    RTask::startMe();

    if(!isRunning())
    {
        start();
    }
    else
    {
        G_RecvCondition.wakeOne();
    }
}

void MsgReceiveProcTask::runMe()
{
    RTask::runMe();
}

void MsgReceiveProcTask::run()
{
    runningFlag = true;

    while(runningFlag)
    {
        if(G_RecvButts.isEmpty())
        {
            G_RecvMutex.lock();
            G_RecvCondition.wait(&G_RecvMutex);
            G_RecvMutex.unlock();
        }

        if(runningFlag && G_RecvButts.size() > 0)
        {
            G_RecvMutex.lock();
            QByteArray array = G_RecvButts.dequeue();
            G_RecvMutex.unlock();

            if(array.size() > 0)
            {
                validateRecvData(array);
            }
        }
    }
}

void MsgReceiveProcTask::validateRecvData(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data,&jsonParseError);
    if(jsonParseError.error == QJsonParseError::NoError)
    {
        QJsonObject root = document.object();

//        int magicNum =  root.value(context.magicNum).toInt();
//        if(magicNum == 0xABCDDCBA)
        {
            switch(root.value(JsonKey::key(JsonKey::Type)).toInt())
            {
                case MSG_CONTROL:
                                    handleCommandMsg((MsgCommand)root.value(JsonKey::key(JsonKey::Command)).toInt(),root.value(JsonKey::key(JsonKey::Data)).toObject());
                                    break;
                case MSG_TEXT:
                                break;
                case MSG_IMAGE:
                                break;
                case MSG_FILE:
                                break;
                default:
                    break;
            }
        }
    }
}

/*!
     * @brief 处理命令消息
     * @param[in] commandType 命令类型
     * @param[in] obj 数据主体
     * @return 无
     */
void MsgReceiveProcTask::handleCommandMsg(MsgCommand commandType, QJsonObject obj)
{
    switch(commandType)
    {
        case MSG_USER_REGISTER:
                            RSingleton<DataProcess>::instance()->proRegistResponse(obj);
                            break;
        case MSG_USER_LOGIN:
                            RSingleton<DataProcess>::instance()->proLoginResponse(obj);
                            break;
    };
}


void MsgReceiveProcTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    G_RecvCondition.wakeOne();
}
