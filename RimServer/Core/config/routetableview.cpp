#include "routetableview.h"

#include <QHeaderView>
#include <QContextMenuEvent>
#include <QCursor>
#include <QDebug>

#include "../constants.h"
#include "../actionmanager/actionmanager.h"

RouteTableView::RouteTableView(ViewType type, QWidget *parent):
    QTableView(parent),vtype(type)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    horizontalHeader()->setStretchLastSection(true);

    setStyleSheet(QString(QLatin1String("QTreeView{ border:none; background:white; } \
                               QTreeView::item:selected{ color:white; background:%1; }")).arg(Constant::SELECTED_LINE_COLOR));


}

RouteTableView::~RouteTableView()
{

}

void RouteTableView::contextMenuEvent(QContextMenuEvent *event)
{
    if(vtype == V_SERVER){
        ActionManager::instance()->menu(Constant::MENU_ROUTE_TABLE_SERVER)->exec(QCursor::pos());
    }else if(vtype == V_CLIENT){
        ActionManager::instance()->menu(Constant::MENU_ROUTE_TABLE_CLIENT)->exec(QCursor::pos());
    }
}
