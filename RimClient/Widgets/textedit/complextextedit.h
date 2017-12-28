/*!
 *  @brief     聊天对话窗口
 *  @details   显示双方的聊天内容，支持显示头像、图片、文字样式、历史记录等
 *  @file      complextextedit.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.22
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef COMPLEXTEXTEDIT_H
#define COMPLEXTEXTEDIT_H

#include "basetextedit.h"

class ComplexTextEdit : public BaseTextEdit
{
public:
    ComplexTextEdit(QWidget * parent = 0);
    ~ComplexTextEdit();
};

#endif // COMPLEXTEXTEDIT_H
