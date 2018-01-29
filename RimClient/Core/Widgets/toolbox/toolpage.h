/*!
 *  @brief     工具页面
 *  @details   工具箱中的每一项，支持详细和缩略显示
 *  @file      toolpage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.18
 *  @warning
 *  @copyright NanJing RenGu.
 *  @change
 *      date:20180118   content:添加获取page的文本框位置信息方法textRect    name:LYS
 *      date:20180122   content:添加获取page的文本框内容方法toolName      name:LYS
 *      date:20180129   content:添加移除page中的目标item方法removeItem   name:LYS
 *      date:20180129   content:添加page的item移除信号itemRemoved   name:LYS
 */
#ifndef TOOLPAGE_H
#define TOOLPAGE_H

#include <QWidget>

#include "toolitem.h"
#include "datastruct.h"

class ToolBox;
class ToolPagePrivate;

using namespace GroupPerson;

class ToolPage : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolPage)
public:
    ToolPage(ToolBox * parent = 0);
    ~ToolPage();

    void setToolName(QString text);

    void addItem(ToolItem *item);
    QList<ToolItem *> &items();

    bool removeItem(ToolItem * item);

    void setMenu(QMenu * menu);

    QRect textRect()const;
    QString toolName()const;
    int txtFixedHeight();
    const PersonGroupInfo & pageInfo();
    void setSortNum(const int num);

public slots:
    void updateGroupActions();

signals:
    void selectedPage(ToolPage *);
    void clearItemSelection(ToolItem*);
    void currentPosChanged();
    void updateGroupActions(ToolPage *);
    void itemRemoved(ToolItem*);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    ToolPagePrivate * d_ptr;
};

#endif // TOOLPAGE_H
