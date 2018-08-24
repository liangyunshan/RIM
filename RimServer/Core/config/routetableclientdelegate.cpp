#include "routetableclientdelegate.h"

#include <QLineEdit>
#include <QComboBox>

#include "routetablehead.h"
#include "../global.h"
#include "Util/scaleswitcher.h"
#include "../protocol/datastruct.h"

RouteTableClientDelegate::RouteTableClientDelegate(QObject *parent):
    QItemDelegate(parent)
{

}

RouteTableClientDelegate::~RouteTableClientDelegate()
{

}

QWidget *RouteTableClientDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch(index.column()){
        case C_C_NODE:
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

void RouteTableClientDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit * edit = dynamic_cast<QLineEdit *>(editor);
    ParameterSettings::NodeClient * client = static_cast<ParameterSettings::NodeClient *>(index.internalPointer());
    if(edit && client){
        switch(index.column()){
            case C_C_NODE:
                    {
                        edit->setText(ScaleSwitcher::fromDecToHex(client->nodeId));
                    }
                    break;
            default:
                break;
        }
    }
}

void RouteTableClientDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit * edit = dynamic_cast<QLineEdit *>(editor);
    ParameterSettings::NodeClient * client = static_cast<ParameterSettings::NodeClient *>(index.internalPointer());
    if(edit && client){
        switch(index.column()){
            case C_C_NODE:
                    {
                        client->nodeId = ScaleSwitcher::fromHexToDec(edit->text());
                    }
                    break;
            default:
                break;
        }
    }
}

void RouteTableClientDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(editor)
        editor->setGeometry(option.rect);
}
