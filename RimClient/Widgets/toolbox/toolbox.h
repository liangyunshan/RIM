/*!
 *  @brief     工具箱控件
 *  @details   自定义工具箱控件，实现抽屉效果，支持拖拽、多层展开等操作
 *  @file      toolbox.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.18
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>

#include "toolpage.h"

class ToolBoxPrivate;
class ToolPage;
class QMenu;

class ToolBox : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolBox)
public:
    explicit ToolBox(QWidget *parent = 0);

    ToolPage *addPage(QString text);

    ToolPage * selectedPage();

    void setSelectedItem(ToolItem * item);
    ToolItem * selectedItem();

    int pageCount();

    void setContextMenu(QMenu * menu);

private slots:
    void setSlectedPage(ToolPage*);

protected:
    void contextMenuEvent(QContextMenuEvent * event);

private:
    ToolBoxPrivate * d_ptr;

};

#endif // TOOLBOX_H
