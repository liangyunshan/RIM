/*!
 *  @brief     单行文本编辑
 *  @details   1.调整为鼠标点击后，切换光标显示；
 *  @file      rlineedit.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.02.07
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RLINEEDIT_H
#define RLINEEDIT_H

#include <QLineEdit>
#include "Widgets/toolbox/toolitem.h"

class RLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    RLineEdit(QWidget * parent = 0);
    ~RLineEdit();

protected:
    bool event(QEvent * eve);

signals:
    void contentChanged(QString content);

private:
    void setFocusIn(ToolItem::ItemState state);

private:
    ToolItem::ItemState itemState;
    QString originContent;

};

#endif // RLINEEDIT_H
