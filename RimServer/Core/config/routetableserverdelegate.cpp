#include "routetableserverdelegate.h"

#include <QWidget>
#include <QLineEdit>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QRegExp>
#include <QDebug>

#include "Util/scaleswitcher.h"
#include "routetablehead.h"
#include "global.h"
#include "../protocol/datastruct.h"

RouteTableServerDelegate::RouteTableServerDelegate(QObject *parent):
    QItemDelegate(parent)
{

}

RouteTableServerDelegate::~RouteTableServerDelegate()
{

}

QWidget *RouteTableServerDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch(index.column()){
        case C_S_NODE:
        case C_S_IP:
        case C_S_PORT:
                {
                    QLineEdit * edit = new QLineEdit(parent);
                    return edit;
                }
            break;
        default:
            break;
    }
    return NULL;
}

void RouteTableServerDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit * edit = dynamic_cast<QLineEdit *>(editor);
    ParameterSettings::NodeServer * server = static_cast<ParameterSettings::NodeServer *>(index.internalPointer());
    if(edit && server){
        switch(index.column()){
            case C_S_NODE:
                    {
                        edit->setText(ScaleSwitcher::fromDecToHex(server->nodeId));
                    }
                    break;
            case C_S_IP:
                    {
                        edit->setText(server->localIp);
                    }
                    break;
            case C_S_PORT:
                    {
                        edit->setText(server->localPort);
                    }
                    break;
            default:
                break;
        }
    }
}

void RouteTableServerDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit * edit = dynamic_cast<QLineEdit *>(editor);
    ParameterSettings::NodeServer * server = static_cast<ParameterSettings::NodeServer *>(index.internalPointer());
    if(edit && server){
        //TODO 20180822 加入对数据的验证
        switch(index.column()){
            case C_S_NODE:
                    {
                        if(edit->text().size() == 0 || edit->text().replace(QRegExp("\s*"),"").size() == 0)
                            return;
                        server->nodeId = ScaleSwitcher::fromHexToDec(edit->text());
                    }
                    break;
            case C_S_IP:
                    {
                        server->localIp = edit->text();
                    }
                    break;
            case C_S_PORT:
                    {
                        server->localPort = edit->text();
                    }
                    break;
            default:
                break;
        }
    }
}

void RouteTableServerDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(editor)
        editor->setGeometry(option.rect);
}
