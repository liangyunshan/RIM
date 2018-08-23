/*!
 *  @brief     路由表数据模型
 *  @details   显示服务器与节点的关系，提供可视化编辑窗口
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.17
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef ROUTETABLESERVERMODEL_H
#define ROUTETABLESERVERMODEL_H

#include <QAbstractTableModel>

class RouteSettings;

class RouteTableServerModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    RouteTableServerModel(QObject * parent = 0);
    ~RouteTableServerModel();

    void setRouteData(RouteSettings * data);

protected:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;

private:
    void refreshData();

private:
    RouteSettings * settings;

};

#endif // ROUTETABLEMODEL_H
