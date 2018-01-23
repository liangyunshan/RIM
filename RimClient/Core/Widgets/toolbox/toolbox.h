/*!
 *  @brief     工具箱控件
 *  @details   自定义工具箱控件，实现抽屉效果，支持拖拽、多层展开等操作
 *  @file      toolbox.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.18
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QWidget>
#include <QStringList>

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
    ToolItem * selectedItem();

    int pageCount();

    void testMe();

    void setContextMenu(QMenu * menu);
    QStringList toolPageSNames()const;

    const QStringList toolPageSNames();

private slots:
    void setSlectedPage(ToolPage*);
    void clearItemSelection(ToolItem*item);
    void updateLayout();

protected:
    void contextMenuEvent(QContextMenuEvent *);

private:
    ToolBoxPrivate * d_ptr;

};

#endif // TOOLBOX_H
