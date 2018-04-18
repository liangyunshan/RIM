/*!
 *  @brief     添加好友面板
 *  @details   可按联系人、群进行查找好友
 *  @file      addfriend.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.25
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20170201:wey:添加键盘快捷响应事件
 */

#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include "widget.h"
#include "observer.h"
#include "protocoldata.h"
using namespace ProtocolType;

class AddFriendPrivate;
class ToolItem;

class AddFriend : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AddFriend)
public:
    AddFriend(QWidget * parent = 0);
    ~AddFriend();

    void onMessage(MessageType type);

private:
    void keyPressEvent(QKeyEvent * event);

private slots:
    void startSearch();
    void reSearch();
    void addFriend();
    void itemSelected(ToolItem * item);
    void validateInputState(QString content);
    void recvSearchFriendResponse(ResponseAddFriend status,SearchFriendResponse response);
    void recvAddFriendResponse(ResponseAddFriend status);

private:
    void enableInput(bool flag);

private:
    AddFriendPrivate * d_ptr;
};

#endif // ADDFRIEND_H
