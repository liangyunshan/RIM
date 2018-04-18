#include "userfriendcontainer.h"

UserFriendContainer::UserFriendContainer()
{

}

void UserFriendContainer::reset(QList<RGroupData *> &list)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RGroupData *>::iterator iter = friendList.begin();
    while(iter != friendList.end()){
        delete (*iter);
        iter = friendList.erase(iter);
    }
    friendList.clear();

    friendList = list;
}

bool UserFriendContainer::containUser(const QString accountId)
{
    lock_guard<mutex> guard(lockMutex);

    auto beg = friendList.begin();
    while(beg != friendList.end()){
        if(find_if((*beg)->users.cbegin(),(*beg)->users.cend(),[&](const SimpleUserInfo * info){return info->accountId == accountId;}) != (*beg)->users.cend())
            return true;
        beg++;
    }
    return false;
}

int UserFriendContainer::groupSize()
{
    lock_guard<mutex> guard(lockMutex);
    return friendList.size();
}

RGroupData *UserFriendContainer::element(int index)
{
    lock_guard<mutex> guard(lockMutex);
    if(index >=0 && index < friendList.size())
        return friendList.at(index);
    return NULL;
}

RGroupData *UserFriendContainer::element(const QString groupId)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RGroupData *>::iterator iter = find_if(friendList.begin(),friendList.end(),[&](const RGroupData * data){return data->groupId == groupId;});
    if(iter != friendList.end())
        return (*iter);

    return nullptr;
}

/*!
 * @brief 移除指定的分组，将分组中的子项添加至默认分组
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
bool UserFriendContainer::deleteGroup(const QString &groupId)
{
    lock_guard<mutex> guard(lockMutex);
    QList<RGroupData *>::iterator defaultIter = std::find_if(friendList.begin(),friendList.end(),[](const RGroupData * data){
        if(data->isDefault)
            return true;
        return false;
    });

    QList<RGroupData *>::iterator delIter = std::find_if(friendList.begin(),friendList.end(),[=](const RGroupData * data){
        if(data->groupId == groupId)
            return true;
        return false;
    });

    if(defaultIter != friendList.end() && delIter != friendList.end()){
        (*defaultIter)->users.append((*delIter)->users);
        (*delIter)->users.clear();
        delete (*delIter);
        friendList.erase(delIter);
        return true;
    }
    return false;
}

/*!
 * @brief 删除指定分组中的指定联系人
 * @param[in] groupId 分组ID
 * @param[in] accountId 待删除的用户ID
 * @return 是否删除成功
 */
bool UserFriendContainer::deleteUser(const QString groupId, const QString &accountId)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RGroupData *>::iterator iter  = std::find_if(friendList.begin(),friendList.end(),[&groupId](const RGroupData * data){
        if(groupId == data->groupId)
            return true;
        return false;
    });

    if(iter != friendList.end()){
        QList<SimpleUserInfo *>::iterator siter = (*iter)->users.begin();
        while(siter != (*iter)->users.end()){
            if((*siter)->accountId == accountId){
                delete (*siter);
                (*iter)->users.erase(siter);
                return true;
            }
            siter++;
        }
    }
    return false;
}

/*!
 * @brief 向指定分组中添加用户
 * @param[in] groupId 分组ID
 * @param[in] userInfo 待添加用户信息
 * @return 是否添加成功
 */
bool UserFriendContainer::addUser(const QString groupId, SimpleUserInfo &userInfo)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RGroupData *>::iterator groupIter =  friendList.begin();
    while(groupIter != friendList.end())
    {
        if((*groupIter)->groupId == groupId)
        {
            SimpleUserInfo * info = new SimpleUserInfo;
            info->accountId = userInfo.accountId;
            info->nickName = userInfo.nickName;
            info->signName = userInfo.signName;
            info->isSystemIcon = userInfo.isSystemIcon;
            info->iconId = userInfo.iconId;
            info->remarks = userInfo.remarks;
            info->status = userInfo.status;
            (*groupIter)->users.append(info);
            return true;
        }
        groupIter++;
    }

    return false;
}

const QList<RGroupData *> &UserFriendContainer::list()
{
    unique_lock<mutex> ul(lockMutex);
    return friendList;
}

void UserFriendContainer::clear()
{

}
