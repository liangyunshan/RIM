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

GroupClient *GroupManager::addClient(QString chatId)
{
    QMutexLocker locker(&mutex);

    if(clients.contains(chatId))
        return clients.value(chatId);

    GroupClient * client = new GroupClient;
    clients.insert(chatId,client);

    return client;
}

bool GroupManager::removeClient(QString chatId)
{
    QMutexLocker locker(&mutex);
    if(clients.contains(chatId)){
        delete clients.value(chatId);
        int t_result = clients.remove(chatId);
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

GroupClient *GroupManager::client(QString chatId)
{
    QMutexLocker locker(&mutex);
    QHash<QString,GroupClient*>::iterator iter = clients.begin();
    while(iter != clients.end()){
        if(iter.value()->simpleChatInfo.chatId == chatId)
            return iter.value();
        iter++;
    }

    return NULL;
}

GroupClient *GroupManager::clientByChatRoomId(QString chatRoomId)
{
    QMutexLocker locker(&mutex);
    QHash<QString,GroupClient*>::iterator iter = clients.begin();
    while(iter != clients.end()){
        if(iter.value()->simpleChatInfo.chatRoomId == chatRoomId)
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
