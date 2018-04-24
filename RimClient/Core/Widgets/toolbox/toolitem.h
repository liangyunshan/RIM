/*!
 *  @brief     工具栏条目
 *  @details   1.用于在联系人信息、群信息、历史聊天信息面板中显示用户信息；
 *             2.用于在好友查询结果时，显示查询结果的简单描述；
 *             3.用于在系统通知窗口中，显示通知信息；
 *  @file      toolitem.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.19
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20180209:LYS:添加置联系人item中头像为灰色或正常状态方法setStatus
 *             20180224:wey:添加item在线状态；
 *             20180305:LYS:鼠标右键选中item后更新当前的item
 *             20180424:wey:调整item布局，新增timeLabel
 */
#ifndef TOOLITEM_H
#define TOOLITEM_H

#include <QWidget>

#include "datastruct.h"

class QMenu;
class QLabel;
class QImage;
class ToolItemPrivate;
class ToolPage;

class ToolItem : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolItem)
public:
    explicit ToolItem(ToolPage *page,QWidget * parent = 0);
    ~ToolItem();

    void setIcon(QString icon);

    void setName(QString name);
    QString getName()const;

    void setNickName(QString name);
    void setDescInfo(QString text);

    void setGroupMemberCoung(int number);
    void setContentMenu(QMenu * contentMenu);

    void setDate(QString desc);

    void addNotifyInfo();
    int getNotifyCount();

    void setChecked(bool flag);
    bool isChecked();

    void setTop(bool flag);
    bool isTop();

    void setStatus(OnlineStatus);
    bool isOnline()const;

    enum ItemState
    {
       Item_Normal,
       Mouse_Enter,
       Mouse_Leave,
       Mouse_Checked,
       Item_FocusIn,
       Item_FocusOut
    };
    Q_FLAG(ItemState)

    enum ItemInfoLabelType
    {
        ItemTextInfo,
        ItemSystemInfo
    };
    Q_FLAG(ItemInfoLabelType)

signals:
    void clearSelectionOthers(ToolItem *);
    void showChatWindow(ToolItem *);
    void itemDoubleClick(ToolItem *);
    void itemMouseHover(bool,ToolItem*);
    void updateGroupActions();

private slots:
    void cursorHoverIcon(bool flag);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void setItemState(ItemState state);

    ToolItemPrivate * d_ptr;
};

#endif // TOOLITEM_H
