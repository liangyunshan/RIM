#include "userchatcontainer.h"

UserChatContainer::UserChatContainer()
{

}

void UserChatContainer::reset(QList<RChatGroupData *> &list)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RChatGroupData *>::iterator iter = groupList.begin();
    while(iter != groupList.end()){
        delete (*iter);
        iter = groupList.erase(iter);
    }
    groupList.clear();

    groupList = list;
}

int UserChatContainer::groupSize()
{
    lock_guard<mutex> guard(lockMutex);
    return groupList.size();
}

RChatGroupData *UserChatContainer::element(int index)
{
    lock_guard<mutex> guard(lockMutex);
    if(index >=0 && index < groupList.size())
        return groupList.at(index);
    return NULL;
}

RChatGroupData *UserChatContainer::element(const QString groupId)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RChatGroupData *>::iterator iter = find_if(groupList.begin(),groupList.end(),[&](const RChatGroupData * data){
        return data->groupId == groupId;});
    if(iter != groupList.end())
        return (*iter);

    return nullptr;
}

/*!
 * @brief 通过群UUID查找对应分组ID
 * @param[in] chatRoomId 群uuid
 * @return 分组ID
 */
QString UserChatContainer::getChatGroupId(const QString chatRoomId)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RChatGroupData *>::iterator iter = find_if(groupList.begin(),groupList.end(),[&](const RChatGroupData * data){
        foreach(const SimpleChatInfo * cinfo,data->chatGroups){
            if(cinfo->chatRoomId == chatRoomId)
                return true;
        }
        return false;
    });
    if(iter != groupList.end())
        return (*iter)->groupId;

    return QString("");
}

/*!
 * @brief 通过群UUID查找群ID
 * @param[in] chatRoomId 群uuid
 * @return 群ID
 */
QString UserChatContainer::getChatId(const QString chatRoomId)
{
    lock_guard<mutex> guard(lockMutex);

    QString chatId;
    QList<RChatGroupData *>::iterator iter = find_if(groupList.begin(),groupList.end(),[&](const RChatGroupData * data){
        foreach(const SimpleChatInfo * cinfo,data->chatGroups){
            if(cinfo->chatRoomId == chatRoomId){
                chatId = cinfo->chatId;
                return true;
            }
        }
        return false;
    });

    return chatId;
}

/*!
 * @brief 移除指定的分组，将分组中的子项添加至默认分组
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否删除成功
 */
bool UserChatContainer::deleteGroup(const QString &groupId)
{
    lock_guard<mutex> guard(lockMutex);
    QList<RChatGroupData *>::iterator defaultIter = std::find_if(groupList.begin(),groupList.end(),[](const RChatGroupData * data){
        if(data->isDefault)
            return true;
        return false;
    });

    QList<RChatGroupData *>::iterator delIter = std::find_if(groupList.begin(),groupList.end(),[=](const RChatGroupData * data){
        if(data->groupId == groupId)
            return true;
        return false;
    });

    if(defaultIter != groupList.end() && delIter != groupList.end()){
        (*defaultIter)->chatGroups.append((*delIter)->chatGroups);
        (*delIter)->chatGroups.clear();
        delete (*delIter);
        groupList.erase(delIter);
        return true;
    }
    return false;
}

void UserChatContainer::addTmpGroup(const QString id, RChatGroupData *data)
{
    tmpCreateOrRenameMap.insert(id,data);
}

void UserChatContainer::addGroup(const QString id, int groupIndex)
{
    lock_guard<mutex> guard(lockMutex);

    if(tmpCreateOrRenameMap.contains(id)){
       RChatGroupData * data = tmpCreateOrRenameMap.value(id);
       data->index = groupIndex;
       tmpCreateOrRenameMap.remove(id);
       groupList.push_back(data);
    }
}

/*!
 * @brief 重命名分组
 * @param[in] id 待重命名的分组uuid
 */
void UserChatContainer::renameGroup(const QString &id)
{
    lock_guard<mutex> guard(lockMutex);

    if(tmpCreateOrRenameMap.contains(id)){
        RChatGroupData * data = tmpCreateOrRenameMap.value(id);
        QList<RChatGroupData *>::iterator iter = std::find_if(groupList.begin(),groupList.end(),[&](const RChatGroupData * gdata){
            if(gdata->groupId == data->groupId)
                return true;
            return false;
        });

        if(iter != groupList.end()){
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
 * @return 是否排序成功
 */
void UserChatContainer::sortGroup(const QString &groupId, int newPageIndex)
{
    lock_guard<mutex> guard(lockMutex);
    int oldPageIndex = -1;
    QList<RChatGroupData *>::iterator iter = std::find_if(groupList.begin(),groupList.end(),[=,&oldPageIndex](const RChatGroupData * data){
        ++oldPageIndex;
        if(data->groupId == groupId)
            return true;
        return false;
    });

    if(iter != groupList.end() && oldPageIndex >=0 && oldPageIndex < groupList.size()
            && newPageIndex >=0  && newPageIndex < groupList.size()){
        groupList.swap(oldPageIndex,newPageIndex);
    }
}

bool UserChatContainer::addChatGroupRoom(const QString groupId,const SimpleChatInfo &chatInfo)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RChatGroupData *>::iterator groupIter =  groupList.begin();
    while(groupIter != groupList.end())
    {
        if((*groupIter)->groupId == groupId)
        {
            SimpleChatInfo * info = new SimpleChatInfo;
            info->id = chatInfo.id;
            info->chatRoomId = chatInfo.chatRoomId;
            info->chatId = chatInfo.chatId;
            info->remarks = chatInfo.remarks;
            info->messNotifyLevel = chatInfo.messNotifyLevel;
            info->isSystemIcon = chatInfo.isSystemIcon;
            info->iconId = chatInfo.iconId;
            (*groupIter)->chatGroups.append(info);
            return true;
        }
        groupIter++;
    }

    return false;
}

bool UserChatContainer::containChatGroupRoom(const QString chatId)
{
    lock_guard<mutex> guard(lockMutex);

    auto beg = groupList.begin();
    while(beg != groupList.end()){
        if(find_if((*beg)->chatGroups.cbegin(),(*beg)->chatGroups.cend(),[&](const SimpleChatInfo * info){return info->chatId == chatId;}) != (*beg)->chatGroups.cend())
            return true;
        beg++;
    }
    return false;
}

bool UserChatContainer::deleteChatGroupRoom(const QString groupId, const QString &chatId)
{
    lock_guard<mutex> guard(lockMutex);

    QList<RChatGroupData *>::iterator iter  = std::find_if(groupList.begin(),groupList.end(),[&groupId](const RChatGroupData * data){
        if(groupId == data->groupId)
            return true;
        return false;
    });

    if(iter != groupList.end()){
        QList<SimpleChatInfo *>::iterator siter = (*iter)->chatGroups.begin();
        while(siter != (*iter)->chatGroups.end()){
            if((*siter)->chatId == chatId){
                delete (*siter);
                (*iter)->chatGroups.erase(siter);
                return true;
            }
            siter++;
        }
    }
    return false;
}

bool UserChatContainer::moveChatGroupRoom(const QString &srcGroupId, const QString &destGroupId, const QString &chatId)
{
    unique_lock<mutex> ul(lockMutex);

    QList<RChatGroupData *>::iterator srcIter = std::find_if(groupList.begin(),groupList.end(),[&]
                                                         (RChatGroupData * groupData){return groupData->groupId == srcGroupId;});

    QList<RChatGroupData *>::iterator destIter = std::find_if(groupList.begin(),groupList.end(),[&]
                                                         (RChatGroupData * groupData){return groupData->groupId == destGroupId;});

    if(srcIter != groupList.end() && destIter != groupList.end()){
        auto groupIter = (*srcIter)->chatGroups.begin();
        while(groupIter != (*srcIter)->chatGroups.end()){
            if((*groupIter)->chatId == chatId){
                (*destIter)->chatGroups.push_back(*groupIter);
                (*srcIter)->chatGroups.erase(groupIter);
                return true;
            }
            groupIter ++;
        }
    }

    return false;
}
