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
class ToolItemPrivate;
class ToolPage;

class ToolItem : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolItem)
public:
    explicit ToolItem(ToolPage *page,QWidget * parent = 0);

    void setIcon(QString icon);

    void setName(QString name);
    void setNickName(QString name);
    void setDescInfo(QString text);

    void setGroupMemberCoung(int number);
    void setContentMenu(QMenu * contentMenu);

    bool isChecked();
    void setChecked(bool flag);

    enum ItemState
    {
       Mouse_Enter,
       Mouse_Leave,
       Mouse_Checked
    };
    Q_FLAG(ItemState)

signals:
    void clearSelectionOthers(ToolItem *);
    void showChatWindow(ToolItem *);
    void itemDoubleClick(ToolItem *);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void setItemState(ItemState state);

    ToolItemPrivate * d_ptr;

};

#endif // TOOLITEM_H
