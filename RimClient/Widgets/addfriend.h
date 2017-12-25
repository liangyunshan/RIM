/*!
 *  @brief     添加好友面板
 *  @details   可按联系人、群进行查找好友
 *  @file      addfriend.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.25
 *  @warning
 *  @copyright GNU Public License.
 */

#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include "widget.h"

class AddFriendPrivate;

class AddFriend : public Widget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AddFriend)
public:
    AddFriend(QWidget * parent = 0);

private slots:
    void startSearch();
    void reSearch();

private:
    AddFriendPrivate * d_ptr;
};

#endif // ADDFRIEND_H
