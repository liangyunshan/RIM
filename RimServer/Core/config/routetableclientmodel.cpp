#include "routetableclientmodel.h"

#include <QColor>

#include "../global.h"
#include "../protocol/datastruct.h"
#include "Util/scaleswitcher.h"
#include "routetablehead.h"
#include "routesettings.h"

RouteTableClientModel::RouteTableClientModel(QObject * parent):
    QAbstractTableModel(parent),currServerNode(-1)
{

}

RouteTableClientModel::~RouteTableClientModel()
{

}

void RouteTableClientModel::setCurrServer(ushort serverNode)
{
    currServerNode = serverNode;
    refreshData();
}

void RouteTableClientModel::setRouteData(RouteSettings *data)
{
    this->settings = data;
    refreshData();
}

int RouteTableClientModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid() && currServerNode < 0)
        return 0;

    ParameterSettings::NodeServer * server = settings->settings()->servers.value(currServerNode);
    if(server)
        return server->clients.size();
    return 0;
}

int RouteTableClientModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid() && currServerNode < 0)
        return 0;

    return C_C_TOTAL_COLUMN;
}

QVariant RouteTableClientModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || currServerNode < 0)
        return QVariant();

    ParameterSettings::NodeServer * server = settings->settings()->servers.value(currServerNode);
    if(server)
    {
        if(index.row() >= server->clients.size())
            return QVariant();

        int row = index.row();
        int column = index.column();

        switch(role){
            case Qt::TextAlignmentRole:
                    return Qt::AlignCenter;
                break;
            case Qt::DisplayRole:
                {
                        switch(column){
                            case C_C_NODE:
                                        return ScaleSwitcher::fromDecToHex(server->clients.at(row)->nodeId);
                                    break;
                            case C_C_COMMUNCATE:
                                        return server->clients.at(row)->communicationMethod;
                                    break;
                            case C_C_FORMAT:
                                        return server->clients.at(row)->messageFormat;
                                    break;
                            case C_C_CHANNEL:
                                        return server->clients.at(row)->channel;
                                    break;
                            case C_C_MESSTYPE:
                                        return server->clients.at(row)->distributeMessageType;
                                    break;
                            default:
                                    break;
                        }
                }
            break;

            case Qt::BackgroundColorRole:
                    return row % 2 == 0 ? QColor(240,240,240,200) : QVariant();
                break;

            default:
                break;
        }
    }
    return QVariant();
}

Qt::ItemFlags RouteTableClientModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag;

    flag |= Qt::ItemIsSelectable;
    flag  |= Qt::ItemIsEnabled;
    flag  |= Qt::ItemIsEditable;

    return flag;
}

QVariant RouteTableClientModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                    case C_C_NODE:
                            return tr("Node id");
                        break;
                    case C_C_COMMUNCATE:
                            return tr("Communication method");
                        break;
                    case C_C_FORMAT:
                            return tr("Message format");
                        break;
                    case C_C_CHANNEL:
                            return tr("Channel");
                        break;
                    case C_C_MESSTYPE:
                            return tr("Distribute message type");
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

QModelIndex RouteTableClientModel::index(int row, int column, const QModelIndex &parent) const
{
    ParameterSettings::NodeServer * server = settings->settings()->servers.value(currServerNode);
    if(row >= 0 && server && row < server->clients.size())
    {
        return createIndex(row,column,server->clients.at(row));
    }
    return QModelIndex();
}

void RouteTableClientModel::refreshData()
{
    beginResetModel();
    endResetModel();
}
