/*!
 *  @brief     群信息界面
 *  @details   显示登录用户群信息
 *  @attention 此功能页面与PanelPersonPage页面功能相似，代码可参考
 *  @file      panelgrouppage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @change
 *      date:20180122:LYS:添加群分组重命名
 *      date:20180202:LYS:添加增加群分组功能
 *           20180424:wey:完善分组功能操作(新建分组、重命名、删除、移动分组，并且与服务器同步)
 */
#ifndef PANELGROUPPAGE_H
#define PANELGROUPPAGE_H

#include <QWidget>
#include <QEvent>
#include "observer.h"
#include "protocoldata.h"

class PanelGroupPagePrivate;
class ToolItem;
class ToolPage;

class PanelGroupPage : public QWidget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelGroupPage)
public:
    explicit PanelGroupPage(QWidget *parent = 0);
    ~PanelGroupPage();

    void onMessage(MessageType type);

signals:
    void registChatGroupResult(bool);

private slots:
    void updateGroupList(MsgOperateResponse status,ChatGroupListResponse* response);

    void respGroupCreate();
    void respGroupRename();
    void respGroupDeleted();
    void respGroupMoved(int index,QString pageId);
    void searchGroup();

    void createNewGroup();
    void respRegistDestoryed(QObject *);

    void sendMessage();
    void viewDetail();
    void modifyGroupInfo();
    void exitGroup();

    void recvGroupPageOperate(MsgOperateResponse status, GroupingResponse response);
    void recvGroupItemOperate(MsgOperateResponse result , GroupingChatResponse response);
    void recvRegistGroup(MsgOperateResponse status, RegistGroupResponse response);

    void recvGroupCommand(MsgOperateResponse result,GroupingCommandResponse response);

    void respItemButtonClick(QEvent::Type type,ToolItem* item);

public slots:
    void renameEditFinished();
    void updateGroupActions(ToolPage *);
    void moveGroupTo();

private:
    void createAction();
    void createGroup();
    void updateGroupDescInfo();
    void updateGroupDescInfo(ToolPage *page);
    void clearTargetGroup(const QString id);
    ToolItem * ceateItem(SimpleChatInfo * chatInfo, ToolPage * page);
    void updateChatShow(const SimpleChatInfo & info);
    void removeContact(const QString chatRoomId);

private:
    PanelGroupPagePrivate * d_ptr;
};

#endif // PANELGROUPPAGE_H
