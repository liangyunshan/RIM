/*!
 *  @brief     简单文本编辑区
 *  @details   输入聊天内容，按快捷键Enter或Ctrl+Enter发送
 *  @file      simpletextedit.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.22
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SIMPLETEXTEDIT_H
#define SIMPLETEXTEDIT_H

#include "basetextedit.h"

class SimpleTextEdit : public BaseTextEdit
{
    Q_OBJECT
public:
    SimpleTextEdit(QWidget * parent = 0 );
    ~SimpleTextEdit();

    void clear();

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void sigEnter();

private slots:
    void slot_TextChanged();

private:
    QColor m_Fontcolor;
};

#endif // SIMPLETEXTEDIT_H
