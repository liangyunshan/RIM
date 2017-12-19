/*!
 *  @brief     工具栏条目
 *  @details   显示联系人信息、群信息、历史聊天信息
 *  @file      toolitem.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.19
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef TOOLITEM_H
#define TOOLITEM_H

#include <QWidget>

#include "datastruct.h"

class QMenu;
class QLabel;
class ToolItem;
class ToolPage;

class ToolItemPrivate : public GlobalData<ToolItem>
{
    Q_DECLARE_PUBLIC(ToolItem)

protected:
    ToolItemPrivate(ToolItem * q):q_ptr(q)
    {
        initWidget();
        checked = false;
        contenxMenu = NULL;
    }

    ToolItem * q_ptr;

    ToolPage * pagePtr;

    void initWidget();

    QWidget * contentWidget;
    QLabel * iconLabel;                 //个人、群组、历史聊天的头像
    QLabel * nameLabel;                 //个人、群组、历史聊天的用户名
    QLabel * nickLabel;                 //个人昵称、群组成员数量
    QLabel * descLabel;                 //个人签名、群组和历史聊天的聊天记录信息
    QLabel * infoLabel;                 //群组和历史聊天的日期

    QMenu * contenxMenu;

    bool checked;                       //是否被选中
};

class ToolItem : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolItem)
public:
    explicit ToolItem(ToolPage *page,QWidget * parent = 0);

    void setIcon(QString icon);
    QString icon();

    void setName(QString name);
    void setNickName(QString name);
    void setDescInfo(QString text);

    void setGroupMemberCoung(int number);
    void setContentMenu(QMenu * contentMenu);

    bool isChecked();
    void setChecked(bool flag);

signals:
    void clearSelectionOthers(ToolItem *);
    void showChatWindow(ToolItem *);

protected:
    bool eventFilter(QObject *watched, QEvent *event);


private:
    ToolItemPrivate * d_ptr;

};

#endif // TOOLITEM_H
