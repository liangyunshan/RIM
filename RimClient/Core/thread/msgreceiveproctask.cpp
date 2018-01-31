#include "msgreceiveproctask.h"

#include <QJsonDocument>

#include "network/netglobal.h"
#include "dataprocess.h"
#include "rsingleton.h"
#include "head.h"
#include "jsonkey.h"

MsgReceiveProcTask::MsgReceiveProcTask(QObject *parent):
    ClientNetwork::RTask(parent)
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

void MsgReceiveProcTask::run()
{
    runningFlag = true;

    while(runningFlag)
    {
        while(G_RecvButts.isEmpty())
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
#ifndef __NO_SQL_PRINT__
        qDebug()<<document.toJson(QJsonDocument::Indented);
#endif
        switch(root.value(JsonKey::key(JsonKey::Type)).toInt())
        {
            case MSG_CONTROL:
                handleCommandMsg((MsgCommand)root.value(JsonKey::key(JsonKey::Command)).toInt(),root);
                break;
            case MSG_TEXT:
                handleTextMsg(root);
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

/*!
     * @brief 处理命令消息
     * @param[in] commandType 命令类型
     * @param[in] obj 数据主体
     * @return 无
     */
void MsgReceiveProcTask::handleCommandMsg(MsgCommand commandType, QJsonObject &obj)
{
    switch(commandType)
    {
        case MSG_USER_REGISTER:
                            RSingleton<DataProcess>::instance()->proRegistResponse(obj);
                            break;
        case MSG_USER_LOGIN:
                            RSingleton<DataProcess>::instance()->proLoginResponse(obj);
                            break;
        case MSG_USER_UPDATE_INFO:
                            RSingleton<DataProcess>::instance()->proUpdateBaseInfoResponse(obj);
                            break;
        case MSG_RELATION_SEARCH:
                            RSingleton<DataProcess>::instance()->proSearchFriendResponse(obj);
                            break;
        case MSG_REALTION_ADD:
                            RSingleton<DataProcess>::instance()->proAddFriendResponse(obj);
                            break;
        case MSG_RELATION_OPERATE:
                            RSingleton<DataProcess>::instance()->proOperateFriendResponse(obj);
                            break;
        case MSG_RELATION_LIST:
                            RSingleton<DataProcess>::instance()->proFriendListResponse(obj);
                            break;
        case MSG_GROUPING_OPERATE:
                            RSingleton<DataProcess>::instance()->proGroupingOperateResponse(obj);
                            break;
    };
}

void MsgReceiveProcTask::handleTextMsg(QJsonObject &obj)
{
    RSingleton<DataProcess>::instance()->proText(obj);
}


void MsgReceiveProcTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    G_RecvCondition.wakeOne();
}
