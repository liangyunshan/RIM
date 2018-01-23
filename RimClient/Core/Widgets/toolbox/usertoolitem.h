/*!
 *  @brief     个人信息item
 *  @details   在ToolItem基础上增强了右键操作
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.XX
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef USERTOOLITEM_H
#define USERTOOLITEM_H

#include "toolitem.h"

class UserToolItemPrivate;
class QMenu;

class UserToolItem : public ToolItem
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(UserToolItem)
public:
    UserToolItem(QWidget * parent = 0);

protected:
    void setContentMenu(QMenu * contentMenu);

private:
    UserToolItemPrivate * d_ptr;
};

#endif // USERTOOLITEM_H
