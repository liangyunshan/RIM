#include "routetableservermodel.h"

#include <QColor>
#include <QDebug>

#include "../protocol/datastruct.h"
#include "routetablehead.h"
#include "Util/scaleswitcher.h"
#include "routesettings.h"

RouteTableServerModel::RouteTableServerModel(QObject *parent):
    QAbstractTableModel(parent)
{

}

RouteTableServerModel::~RouteTableServerModel()
{

}

void RouteTableServerModel::setRouteData(RouteSettings *data)
{
    this->settings = data;
    refreshData();
}

int RouteTableServerModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return settings->settings()->servers.size();
}

int RouteTableServerModel::columnCount(const QModelIndex &parent) const
{
    return C_S_TOTAL_COLUMN;
}

QVariant RouteTableServerModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(index.row() >= settings->settings()->servers.size())
        return QVariant();

    QList<ushort> keys = settings->settings()->servers.keys();
    int row = index.row();
    int column = index.column();

    switch(role){
        case Qt::TextAlignmentRole:
                return Qt::AlignCenter;
            break;
        case Qt::DisplayRole:
            {
                ParameterSettings::NodeServer * server = settings->settings()->servers.value(keys.at(row));
                if(server){
                    switch(column){
                        case C_S_NODE:
                                    return ScaleSwitcher::fromDecToHex(server->nodeId);
                                break;
                        case C_S_IP:
                                    return server->localIp;
                                break;
                        case C_S_PORT:
                                    return server->localPort;
                                break;
                        default:
                                break;
                    }
                }
            }
        break;

        case Qt::BackgroundColorRole:
                return row % 2 == 0 ? QColor(240,240,240,200) : QVariant();
            break;

        default:
            break;
    }

    return QVariant();
}

bool RouteTableServerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    return true;
}

Qt::ItemFlags RouteTableServerModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag;

    flag |= Qt::ItemIsSelectable;
    flag  |= Qt::ItemIsEnabled;

    flag |= Qt::ItemIsEditable;
    return flag;
}

QVariant RouteTableServerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return QVariant();

    switch(role){
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
            break;

        case Qt::DisplayRole:
            {
                switch(section){
                    case C_S_NODE:
                            return tr("Node id");
                        break;
                    case C_S_IP:
                            return tr("Ip address");
                        break;
                    case C_S_PORT:
                            return tr("Port");
                        break;
                    default:
                        break;
                }
            }
            break;

        default:
            break;
    }

    return QVariant();
}

QModelIndex RouteTableServerModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row >= 0 && row < settings->settings()->servers.size())
    {
        QList<ushort> keys = settings->settings()->servers.keys();
        return createIndex(row,column,settings->settings()->servers.value(keys.at(row)));
    }
    return QModelIndex();
}

void RouteTableServerModel::refreshData()
{
    beginResetModel();
    endResetModel();
}
