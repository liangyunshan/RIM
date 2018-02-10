/*!
 *  @brief     工具箱控件
 *  @details   自定义工具箱控件，实现抽屉效果，支持拖拽、多层展开等操作
 *  @file      toolbox.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.18
 *  @warning
 *  @copyright NanJing RenGu.
 *  @change
 *      date:20180129   content:添加获取倒数第二个page方法penultimatePage   name:LYS
 *      date:20180129   content:添加根据目标uuid获取目标page的方法targetPage   name:LYS
 *      date:20180129   content:添加处理page的SIGNAL：itemRemoved(ToolItem*)的槽函数   name:LYS
 *      date:20180131   content:添加移除分组方法removePage   name:LYS
 *      date:20180131   content:添加获取默认分组方法defaultPage   name:LYS
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

    ToolPage * addPage(QString text);
    bool removePage(ToolPage *);
    bool removeFromList(ToolPage *);

    ToolPage * selectedPage();
    ToolItem * selectedItem();

    int pageCount();
    QList<ToolPage *> & allPages()const;

    void setContextMenu(QMenu * menu);
    const QList<PersonGroupInfo> toolPagesinfos();
    ToolPage *penultimatePage();
    ToolPage *defaultPage();
    ToolPage *targetPage(QString &);

signals:
    void updateGroupActions(ToolPage *);
    void noticePageRemoved(ToolPage *);

private slots:
    void setSlectedPage(ToolPage*);
    void clearItemSelection(ToolItem*item);
    void setGroupActions(ToolPage *);
    void itemRemoved(ToolItem *);
    void pageRemoved(ToolPage *);

private:
    void indexInLayout(int,int &);
    ToolPage *dragedPage(const QPoint &);

protected:
    void contextMenuEvent(QContextMenuEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    ToolBoxPrivate * d_ptr;

};

#endif // TOOLBOX_H
