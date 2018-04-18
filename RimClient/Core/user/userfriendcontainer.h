/*!
 *  @brief     用户好友列表
 *  @details   负责管理登陆用户的好友列表信息，此数据与服务器保持一致，在增加、删除、修改好友或分组时均应更新此容器
 *  @author    wey
 *  @version   1.0
 *  @date      2018.04.18
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef USERFRIENDCONTAINER_H_2018_04_18
#define USERFRIENDCONTAINER_H_2018_04_18

#include <QList>
#include <mutex>
using namespace std;

#include "protocoldata.h"
using namespace ProtocolType;

class UserFriendContainer
{
public:
    UserFriendContainer();

    void reset(QList<RGroupData *> & list);
    bool containUser(const QString accountId);

    int groupSize();
    RGroupData * element(int index);
    RGroupData * element(const QString groupId);

    bool deleteGroup(const QString & groupId);
    bool deleteUser(const QString groupId, const QString & accountId);

    bool addUser(const QString groupId,SimpleUserInfo & userInfo);

    const QList<RGroupData *> & list();

    void clear();

private:
    mutex lockMutex;

    QList<RGroupData *> friendList;
};

#endif // USERFRIENDCONTAINER_H_2018_04_18
