/*!
 *  @brief     用户群信息列表
 *  @details   负责管理登陆用户的群列表信息，此数据与服务器保持一致，在增加、删除、修改群或分组时均应更新此容器
 *             【!!!以下操作时务必更新此容器!!!】
 *             分组操作:添加分组、修改分组、移动分组、删除分组
 *             群操作:添加群、修改群、移动群、删除群
 *  @author    wey
 *  @version   1.0
 *  @date      2018.04.24
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef USERCHATCONTAINER_H_04_24
#define USERCHATCONTAINER_H_04_24

#include <QList>
#include <QPair>
#include <QMap>
#include <mutex>
using namespace std;

#include "protocoldata.h"
using namespace ProtocolType;


class UserChatContainer
{
public:
    UserChatContainer();

    void reset(QList<RChatGroupData *> &list);

    int groupSize();

    RChatGroupData * element(int index);
    RChatGroupData * element(const QString groupId);

    bool deleteGroup(const QString & groupId);
    void addTmpGroup(const QString id, RChatGroupData *data);
    void addGroup(const QString id,int groupIndex);
    void renameGroup(const QString &id);
    void sortGroup(const QString & groupId, int newPageIndex);

private:
    mutex lockMutex;
    QMap<QString,RChatGroupData*> tmpCreateOrRenameMap;
    QList<RChatGroupData *> groupList;

};

#endif // USERCHATCONTAINER_H
