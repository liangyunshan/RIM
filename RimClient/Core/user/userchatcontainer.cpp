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

    if(iter != groupList.end() && oldPageIndex >=0 && oldPageIndex < groupList.size()){
        groupList.swap(oldPageIndex,newPageIndex);
    }
}
