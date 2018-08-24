/*!
 *  @brief     客户端路由模型
 *  @details   保存客户端路由的基本信息，响应RouteTableServerModel的点选
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.21
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef ROUTETABLECLIENTMODEL_H
#define ROUTETABLECLIENTMODEL_H

#include <QAbstractTableModel>

class RouteSettings;

class RouteTableClientModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RouteTableClientModel(QObject * parent = 0);
    ~RouteTableClientModel();

    void setCurrServer(ushort serverNode);
    void setRouteData(RouteSettings * data);

protected:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

private:
    void refreshData();

private:
    int currServerNode;
    RouteSettings * settings;

};

#endif // ROUTETABLECLIENTMODEL_H
