/*!
 *  @brief     服务器节点编辑代理
 *  @details   编辑服务器端具有的属性，提供lineedit，combox等交互方式
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.21
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef ROUTETABLESERVERDELEGATE_H
#define ROUTETABLESERVERDELEGATE_H

#include <QItemDelegate>

class RouteTableServerDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit RouteTableServerDelegate(QObject *parent = Q_NULLPTR);
    ~RouteTableServerDelegate();

protected:
    QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor,
                              QAbstractItemModel *model,
                              const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const;
};

#endif // ROUTETABLESERVERDELEGATE_H
