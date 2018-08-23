/*!
 *  @brief     路由表查看与设置界面
 *  @details   通过此页面可编辑服务器、客户端基本信息，支持添加、删除、修改、保存，但目前保存后的数据，需要重新启动程序方可生效 \n
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.17
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef ROUTETABLE_H
#define ROUTETABLE_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class RouteTable;
}

class RouteTableServerModel;
class RouteTableClientModel;
class RouteTableView;
class RouteSettings;

class RouteTable : public QDialog
{
    Q_OBJECT

public:
    explicit RouteTable(QWidget *parent = 0);
    ~RouteTable();

private:
    void initView();
    void initContextMenu();

private slots:
    void switchServerIndex(QModelIndex index);

    void addNewServerGroup(bool);
    void deleteServerGroup(bool);

    void addNewClientGroup(bool);
    void moveClientGroup(bool);
    void deleteClientGroup(bool);

    void saveFile();
    void cancelSave();

private:
    Ui::RouteTable *ui;

    RouteTableServerModel * serverModel;
    RouteTableView * serverView;

    RouteTableView * clientView;
    RouteTableClientModel * clientModel;

    RouteSettings * currRouteSettings;
};

#endif // ROUTETABLE_H
