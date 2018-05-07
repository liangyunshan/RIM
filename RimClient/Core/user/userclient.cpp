#include "userclient.h"

#include "../Widgets/abstractchatwidget.h"

UserClient::UserClient():toolItem(nullptr),chatWidget(nullptr)
{
}

UserClient::~UserClient()
{
    if(chatWidget && chatWidget->isVisible())
    {
        chatWidget->close();
        delete chatWidget;
        chatWidget = NULL;
    }
}

UserManager::UserManager()
{

}

/*!
 * @brief 添加联系人
 * @param[in] accountId:QString 联系人id
 * @return 联系人:UserClient *
 */
UserClient *UserManager::addClient(QString accountId)
{
    std::lock_guard<std::mutex> lg(clientMutex);
    if(clients.contains(accountId))
        return clients.value(accountId);

    UserClient * client = new UserClient;
    clients.insert(accountId,client);

    return client;
}

/*!
 * @brief 移除联系人
 * @param[in] accountId:QString 联系人id
 * @return 移除联系人结果:bool
 */
bool UserManager::removeClient(QString accountId)
{
    std::lock_guard<std::mutex> lg(clientMutex);
    bool t_removeResult = false;
    if(clients.contains(accountId))
    {
        delete clients.value(accountId);
        int t_result = clients.remove(accountId);
        if(t_result != 0){
            t_removeResult = true;
        }
    }
    return t_removeResult;
}

UserClient *UserManager::client(ToolItem *item)
{
    std::unique_lock<std::mutex> ul(clientMutex);
    QHash<QString,UserClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
        if(iter.value()->toolItem == item)
            return iter.value();
        iter++;
    }

    return NULL;
}

UserClient *UserManager::client(QString accountId)
{
    std::unique_lock<std::mutex> ul(clientMutex);
    QHash<QString,UserClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
        if(iter.value()->simpleUserInfo.accountId == accountId)
            return iter.value();
        iter++;
    }

    return NULL;
}

void UserManager::for_each(std::function<void(UserClient*)> func)
{
    std::lock_guard<std::mutex> lg(clientMutex);
    auto beg = clients.begin();
    while(beg != clients.end()){
        func(beg.value());
        beg++;
    }
}

//TODO 关闭窗口出现问题
void UserManager::closeAllClientWindow()
{
    std::lock_guard<std::mutex> lg(clientMutex);
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
    std::lock_guard<std::mutex> lg(clientMutex);
    return clients.size();
}
