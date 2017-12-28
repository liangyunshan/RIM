/*!
 *  @brief     工具页面
 *  @details   工具箱中的每一项，支持详细和缩略显示
 *  @file      toolpage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.18
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef TOOLPAGE_H
#define TOOLPAGE_H

#include <QWidget>

#include "toolitem.h"

class ToolBox;
class ToolPagePrivate;

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

    void setMenu(QMenu * menu);

signals:
    void selectedPage(ToolPage *);
    void clearItemSelection(ToolItem*);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    ToolPagePrivate * d_ptr;
};

#endif // TOOLPAGE_H
