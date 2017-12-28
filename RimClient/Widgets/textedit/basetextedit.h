/*!
 *  @brief     基础文本编辑
 *  @details
 *  @file      basetextedit.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.22
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef BASETEXTEDIT_H
#define BASETEXTEDIT_H

#include <QPlainTextEdit>

class BaseTextEdit : public QPlainTextEdit
{
public:
    BaseTextEdit(QWidget * parent = 0 );
};

#endif // BASETEXTEDIT_H
