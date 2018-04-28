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
 *      20180417:wey:修复删除好友时，全局列表UserFriendContainter中未删除对应item，造成再次添加时按钮不可用的bug
 *      20180419:wey:添加分组移动排序操作
 */
#ifndef PANELPERSONPAGE_H
#define PANELPERSONPAGE_H

#include <QWidget>
#include "observer.h"
#include "protocoldata.h"

class UserClient;

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
    void userDeleted(ChatMessageType messType, QString accountId);
    void userStateChanged(OnlineStatus state,QString accountId);
    void userInfoChanged(QString remarks,QString accountId);

private slots:
    void updateFriendList(MsgOperateResponse status, FriendListResponse * friendList);
    void refreshList();
    //分组操作
    void respGroupCreate();
    void respGroupRename();
    void respGroupDeleted();
    void respGroupMoved(int index,QString pageId);

    void createChatWindow(ToolItem * item);

    void sendInstantMessage();

    void showChatWindow(ToolItem*item);
    void showChatWindow(ChatMessageType messType, QString accountId);

    void showUserDetail();
    void showUserDetail(ChatMessageType messtype,QString accountId);

    void deleteUser();
    void deleteUser(ChatMessageType messtype,QString accountId);

    void modifyUserInfo();

    void recvUserStateChanged(MsgOperateResponse result, UserStateResponse response);
    void recvFriendItemOperate(MsgOperateResponse result,GroupingFriendResponse response);
    void updateModifyInstance(QObject *);
    void requestModifyRemark(QString remark);

    void updateContactList();

    void recvFriendPageOperate(MsgOperateResponse status, GroupingResponse response);

public slots:
    void renameEditFinished();
    void updateGroupActions(ToolPage *);
    void movePersonTo();

private:
    void createAction();
    void addGroupAndUsers();
    void updateGroupDescInfo(ToolPage * page);
    void updateGroupDescInfo();
    ToolItem * ceateItem(SimpleUserInfo *info, ToolPage *page);
    void clearTargetGroup(const QString id);
    void updateContactShow(const SimpleUserInfo &);
    void removeContact(const SimpleUserInfo &);
    void clearUnrealGroupAndUser();

    void createDetailView(UserClient * client);
    void showOrCreateChatWindow(UserClient * client);
    void sendDeleteUserRequest(UserClient * client , QString groupId);

private:
    PanelPersonPagePrivate * d_ptr;
};

#endif // PANELPERSONPAGE_H
