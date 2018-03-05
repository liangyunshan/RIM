/*!
 *  @brief     用户界面
 *  @details   显示用户的联系人信息
 *  @file      panelpersonpage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright NanJing RenGu
 *  @note
 *      20180118:LYS:添加联系人分组重命名
 *      20180122:LYS:添加移动联系人至其他分组功能
 *      20180202:wey:添加移动好友网络请求与接收处理
 *      20180202:LYS:去掉pages与toolItems列表，toolBox中allPages方法与toolPage中items方法代替
 *      20180205:LYS:修复删除分组bug
 *      20180305:LYS:添加创建好友列表或者刷新好友列表标志m_listIsCreated:bool
 */
#ifndef PANELPERSONPAGE_H
#define PANELPERSONPAGE_H

#include <QWidget>
#include "observer.h"
#include "protocoldata.h"

namespace ProtocolType
{
    class FriendListResponse;
}

class PanelPersonPagePrivate;
class ToolItem;
class ToolPage;

class PanelPersonPage : public QWidget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelPersonPage)
public:
    explicit PanelPersonPage(QWidget * parent =0);
    ~PanelPersonPage();

    void onMessage(MessageType type);

signals:
    void showChatDialog(ToolItem * item);

private slots:
    void refreshList();
    void addGroup();
    void renameGroup();
    void delGroup();

    void createChatWindow(ToolItem * item);

    void sendInstantMessage();
    void showUserDetail();
    void modifyUserInfo();
    void deleteUser();

    void recvRelationFriend(MsgOperateResponse result,GroupingFriendResponse response);
    void updateModifyInstance(QObject *);
    void requestModifyRemark(QString remark);

    void updateDetailInstance(QObject *);
    void updateContactList();

public slots:
    void renameEditFinished();
    void updateGroupActions(ToolPage *);
    void movePersonTo();

private:
    void createAction();
    void addGroupAndUsers();
    ToolItem * ceateItem(SimpleUserInfo *info, ToolPage *page);
    void clearTargetGroup(const QString id);
    void updateContactShow(const SimpleUserInfo &);
    void removeContact(const SimpleUserInfo &);
    void clearUnrealGroupAndUser();

private:
    PanelPersonPagePrivate * d_ptr;
};

#endif // PANELPERSONPAGE_H
