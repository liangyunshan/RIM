/*!
 *  @brief     list容器
 *  @details
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.XX
 *  @warning
 *  @copyright GNU Public License.
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
    ToolItem * selectedItem();

    void setContextMenu(QMenu * menu);

private slots:
    void clearItemSelection(ToolItem*item);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    ListBoxPrivate * d_ptr;

};

#endif // LISTBOX_H
