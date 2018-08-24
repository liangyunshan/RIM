/*!
 *  @brief     路由表数据显示
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.17
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef ROUTETABLEVIEW_H
#define ROUTETABLEVIEW_H

#include <QTableView>

#include "routetablehead.h"

class RouteTableView : public QTableView
{
    Q_OBJECT
public:
    RouteTableView(ViewType type,QWidget * parent = 0);
    ~RouteTableView();

protected:
    void contextMenuEvent(QContextMenuEvent * event);

private:
    ViewType vtype;
};

#endif // ROUTETABLETREEVIEW_H
