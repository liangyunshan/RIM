#include "userclient.h"

#include <QMutexLocker>

UserClient::UserClient()
{
    toolItem = NULL;
    chatWidget = NULL;
}

UserClient::~UserClient()
{

}

UserManager::UserManager()
{

}

/*!
     * @brief 添加联系人
     * @param[in] accountId 联系人账号
     * @return 联系人
     */
UserClient *UserManager::addClient(QString accountId)
{
    QMutexLocker locker(&mutex);

    if(clients.contains(accountId))
    {
        return clients.value(accountId);
    }

    UserClient * client = new UserClient;
    clients.insert(accountId,client);

    return client;
}

/*!
     * @brief 移除联系人联系人
     * @param[in] accountId 联系人账号
     * @return 联系人
     */
bool UserManager::removeClient(QString accountId)
{
    QMutexLocker locker(&mutex);
    if(clients.contains(accountId))
    {
        delete clients.value(accountId);
        clients.remove(accountId);
    }
    return false;
}

UserClient *UserManager::client(ToolItem *item)
{
    QMutexLocker locker(&mutex);
    QHash<QString,UserClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
        if(iter.value()->toolItem == item)
        {
            return iter.value();
        }
        iter++;
    }

    return NULL;
}

//TODO 关闭窗口出现问题
void UserManager::closeAllClientWindow()
{
    QMutexLocker locker(&mutex);

    QHash<QString,UserClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
       if(iter.value()->chatWidget)
       {
//           delete iter.value()->chatWidget;
       }
       iter++;
    }
}

int UserManager::size()
{
    QMutexLocker locker(&mutex);
    return clients.size();
}
