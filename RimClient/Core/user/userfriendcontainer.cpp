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
 * @brief 临时存放创建或重命名的容器，待接收服务器反馈后，根据结果，将其移动至friendList或移除出去
 * @attention 1.保存新创建的分组时，不需释放空间；
 *            2.保存重命名分组时，需要手动释放空间；
 * @param[in] id 分组uuid
 * @param[in] data 分组数据信息
 */
void UserFriendContainer::addTmpGroup(const QString id, RGroupData *data)
{
    tmpCreateOrRenameMap.insert(id,data);
}

void UserFriendContainer::addGroup(const QString id, int groupIndex)
{
    lock_guard<mutex> guard(lockMutex);

    if(tmpCreateOrRenameMap.contains(id)){
       RGroupData * data = tmpCreateOrRenameMap.value(id);
       data->index = groupIndex;
       tmpCreateOrRenameMap.remove(id);
       friendList.push_back(data);
    }
}

/*!
 * @brief 重命名分组
 * @param[in] id 待重命名的分组uuid
 */
void UserFriendContainer::renameGroup(const QString & id)
{
    lock_guard<mutex> guard(lockMutex);

    if(tmpCreateOrRenameMap.contains(id)){
        RGroupData * data = tmpCreateOrRenameMap.value(id);
        QList<RGroupData *>::iterator iter = std::find_if(friendList.begin(),friendList.end(),[&](const RGroupData * gdata){
            if(gdata->groupId == data->groupId)
                return true;
            return false;
        });

        if(iter != friendList.end()){
            (*iter)->groupName = data->groupName;
        }
        delete data;
        tmpCreateOrRenameMap.remove(id);
    }
}

/*!
 * @brief 调整分组顺序
 * @param[in] pageId 新排序的分组ID
 * @param[in] newPageIndex 新排序的分组索引
 * @return 是否插入成功
 */
void UserFriendContainer::sortGroup(const QString & groupId, int newPageIndex)
{
    lock_guard<mutex> guard(lockMutex);
    int oldPageIndex = -1;
    QList<RGroupData *>::iterator iter = std::find_if(friendList.begin(),friendList.end(),[=,&oldPageIndex](const RGroupData * data){
        ++oldPageIndex;
        if(data->groupId == groupId)
            return true;
        return false;
    });

    if(iter != friendList.end() && oldPageIndex >=0 && oldPageIndex < friendList.size()){
        friendList.swap(oldPageIndex,newPageIndex);
    }

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
 * @brief 从源分组移动联系人至目的分组
 * @param[in] srcGroupId 源分组
 * @param[in] destGroupId 目的分组
 * @param[in] accountId 待移动用户id
 * @return 是否移动成功
 */
bool UserFriendContainer::moveUser(const QString &srcGroupId, const QString &destGroupId, const QString &accountId)
{
    unique_lock<mutex> ul(lockMutex);

    QList<RGroupData *>::iterator srcIter = std::find_if(friendList.begin(),friendList.end(),[&]
                                                         (RGroupData * groupData){return groupData->groupId == srcGroupId;});

    QList<RGroupData *>::iterator destIter = std::find_if(friendList.begin(),friendList.end(),[&]
                                                         (RGroupData * groupData){return groupData->groupId == destGroupId;});

    if(srcIter != friendList.end() && destIter != friendList.end()){
        auto userIter = (*srcIter)->users.begin();
        while(userIter != (*srcIter)->users.end()){
            if((*userIter)->accountId == accountId){
                (*destIter)->users.push_back(*userIter);
                (*srcIter)->users.erase(userIter);
                return true;
            }
            userIter ++;
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
