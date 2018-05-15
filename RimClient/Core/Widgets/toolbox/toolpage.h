/*!
 *  @brief     工具页面
 *  @details   工具箱中的每一项，支持详细和缩略显示
 *  @file      toolpage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.18
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180118:LYS:添加获取page的文本框位置信息方法textRect
 *      20180122:LYS:添加获取page的文本框内容方法toolName
 *      20180129:LYS:添加移除page中的目标item方法removeItem
 *      20180129:LYS:添加page的item移除信号itemRemoved
 *      20180131:LYS:添加获取page是否展开方法isExpanded
 *      20180131:LYS:修改鼠标左键点击后设置page为选中状态（L373）
 */
#ifndef TOOLPAGE_H
#define TOOLPAGE_H

#include <QWidget>
#include <functional>

#include "toolitem.h"
#include "datastruct.h"

class ToolBox;
class ToolPagePrivate;

class ToolPage : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolPage)
public:
    ToolPage(ToolBox * parent = 0);
    ~ToolPage();

    void setToolName(const QString &text);
    QString toolName()const;

    void setID(const QString & id);
    QString getID()const;

    void setDefault(const bool isDefault);
    bool isDefault()const;

    void addItem(ToolItem *item);
    bool removeItem(ToolItem * item);
    QList<ToolItem *> &items();
    bool containsItem(std::function<bool(ToolItem *)> func);

    void setMenu(QMenu * menu);

    void setExpand(bool);
    bool isExpanded()const;

    void setDescInfo(const QString & content);

    QRect textRect()const;
    QRect titleRect()const;

    int txtFixedHeight();

    const PersonGroupInfo & pageInfo();

    void highlightShow();
    void unHighlightShow();

    bool containsInSimpleTextWidget(QPoint point);

public slots:
    void updateGroupActions();

signals:
    void selectedPage(ToolPage *);
    void clearItemSelection(ToolItem*);
    void updateGroupActions(ToolPage *);
    void itemRemoved(ToolItem*);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    ToolPagePrivate * d_ptr;
};

#endif // TOOLPAGE_H
