/*!
 *  @brief     工具箱控件
 *  @details   自定义工具箱控件，实现抽屉效果，支持拖拽、多层展开等操作
 *  @file      toolbox.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.18
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180129:LYS:添加获取倒数第二个page方法penultimatePage
 *      20180129:LYS:添加根据目标uuid获取目标page的方法targetPage
 *      20180129:LYS:添加处理page的SIGNAL：itemRemoved(ToolItem*)的槽函数
 *      20180131:LYS:添加移除分组方法removePage
 *      20180131:LYS:添加获取默认分组方法defaultPage
 *      20180208:LYS:修复移动排序算法问题
 *      20180227:LYS:添加根据Item找到Page的重载方法targetPage(ToolItem *)
 *      20180419:wey:修复快速拖动分组时分组显示在页面底部()
 *
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

    void sortPage(const QString & groupId, int newPageIndex);
    void revertSortPage(const QString & groupId);

    ToolPage * selectedPage();
    ToolItem * selectedItem();
    ToolPage * penultimatePage();
    ToolPage * defaultPage();
    ToolPage * targetPage(QString &);
    ToolPage * targetPage(ToolItem *);

    int pageCount();
    QList<ToolPage *> & allPages()const;

    void setContextMenu(QMenu * menu);
    const QList<PersonGroupInfo> toolPagesinfos();

signals:
    void updateGroupActions(ToolPage *);
    void noticePageRemoved(ToolPage *);
    void pageIndexMoved(int newIndex,int oldIndex,QString pageId);

private slots:
    void setSlectedPage(ToolPage*);
    void clearItemSelection(ToolItem*item);
    void setGroupActions(ToolPage *);
    void itemRemoved(ToolItem *);
    void pageRemoved(ToolPage *);

private:
    void pageIndexInLayout(QPoint,int &);
    ToolPage *pageInPos(const QPoint &);
    void highLightTarget(const QPoint &);

protected:
    void contextMenuEvent(QContextMenuEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    ToolBoxPrivate * d_ptr;

    /*!
     *  @brief 排序记录
     *  @details 排序后，记录分组的排序结果，便于撤销排序
     */
    struct SortRecord{
        int newIndex;
        int oldIndex;
    };
};

#endif // TOOLBOX_H
