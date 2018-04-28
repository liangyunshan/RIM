#include "groupclient.h"

#include "Widgets/abstractchatwidget.h"

GroupClient::GroupClient():toolItem(nullptr),chatWidget(nullptr)
{

}

GroupClient::~GroupClient()
{
    if(chatWidget && chatWidget->isVisible())
    {
        chatWidget->close();
        delete chatWidget;
        chatWidget = NULL;
    }
}

/*!
 * @brief 添加群分组
 * @param[in] accountId:QString 分组id
 * @return 联系人:UserClient *
 */
GroupManager::GroupManager()
{

}

GroupClient *GroupManager::addClient(QString accountId)
{
    QMutexLocker locker(&mutex);

    if(clients.contains(accountId))
        return clients.value(accountId);

    GroupClient * client = new GroupClient;
    clients.insert(accountId,client);

    return client;
}

bool GroupManager::removeClient(QString accountId)
{
    QMutexLocker locker(&mutex);
    if(clients.contains(accountId)){
        delete clients.value(accountId);
        int t_result = clients.remove(accountId);
        if(t_result != 0)
            return true;
    }
    return false;
}

GroupClient *GroupManager::client(ToolItem *item)
{
    QMutexLocker locker(&mutex);
    QHash<QString,GroupClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
        if(iter.value()->toolItem == item)
            return iter.value();
        iter++;
    }

    return NULL;
}

GroupClient *GroupManager::client(QString accountId)
{
    QMutexLocker locker(&mutex);
    QHash<QString,GroupClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
        if(iter.value()->simpleChatInfo.chatId == accountId)
            return iter.value();
        iter++;
    }

    return NULL;
}

void GroupManager::closeAllClientWindow()
{

}

int GroupManager::size()
{
    QMutexLocker locker(&mutex);
    return clients.size();
}
