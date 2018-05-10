/*!
 *  @brief     用户好友列表
 *  @details   负责管理登陆用户的好友列表信息，此数据与服务器保持一致，在增加、删除、修改好友或分组时均应更新此容器
 *             【!!!以下操作时务必更新此容器!!!】
 *             分组操作:添加分组、修改分组、移动分组、删除分组
 *             联系人操作:添加联系人、修改联系人、移动联系人、删除联系人
 *  @author    wey
 *  @version   1.0
 *  @date      2018.04.18
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef USERFRIENDCONTAINER_H_2018_04_18
#define USERFRIENDCONTAINER_H_2018_04_18

#include <QList>
#include <QPair>
#include <QMap>
#include <mutex>
using namespace std;

#include "protocoldata.h"
using namespace ProtocolType;

class UserFriendContainer
{
public:
    UserFriendContainer();

    void reset(QList<RGroupData *> & list);

    int groupSize();
    QStringList groupNames();
    QList<QPair<QString,QString>> groupIdAndNames();

    QString groupName(const QString  &userAccountId);
    QPair<QString,QString> groupIdAndName(const QString & userAccountId);

    RGroupData * element(int index);
    RGroupData * element(const QString groupId);

    bool deleteGroup(const QString & groupId);
    void addTmpGroup(const QString &id,RGroupData * data);
    void removeTmpGroup(const QString &id);
    void addGroup(const QString id,int groupIndex);
    void renameGroup(const QString &id);
    QString revertRenameGroup(const QString &id);
    void sortGroup(const QString & groupId, int newPageIndex);

    bool addUser(const QString groupId,SimpleUserInfo & userInfo);
    bool containUser(const QString accountId);
    bool deleteUser(const QString groupId, const QString & accountId);
    bool moveUser(const QString & srcGroupId,const QString &destGroupId,const QString & accountId);

    const QList<RGroupData *> & list();

    void clear();

private:
    mutex lockMutex;
    QMap<QString,RGroupData*> tmpCreateOrRenameMap;
    QList<RGroupData *> friendList;
};

#endif // USERFRIENDCONTAINER_H_2018_04_18
