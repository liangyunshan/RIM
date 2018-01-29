/*!
 *  @brief     list容器
 *  @details   以列表存放聊天对话记录，支持按个、全部删除
 *  @file      listbox.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.XX
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note    20180125:wey:添加删除全部item
 *           20180127:wey:添加删除指定item；修复清空后，再次添加item布局错误问题；
 */
#ifndef LISTBOX_H
#define LISTBOX_H

#include <QWidget>

#include "toolitem.h"

class ListBoxPrivate;

class ListBox : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ListBox)
public:
    explicit ListBox(QWidget *parent = 0);

    void addItem(ToolItem * item);
    QList<ToolItem *> items()const;

    bool removeItem(ToolItem * item);
    void clear();

    ToolItem * selectedItem();

    void setContextMenu(QMenu * menu);

signals:
    void currentItemChanged(ToolItem * item);

private slots:
    void clearItemSelection(ToolItem*item);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void contextMenuEvent(QContextMenuEvent *);

private:
    ListBoxPrivate * d_ptr;

};

#endif // LISTBOX_H
