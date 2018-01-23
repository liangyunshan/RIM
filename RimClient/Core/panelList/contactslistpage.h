#ifndef CONTACTSLISTPAGE_H
#define CONTACTSLISTPAGE_H

#include "../datastruct.h"

#include <QMap>
#include <QMenu>
#include <QLineEdit>
#include <QMouseEvent>
#include <QListWidget>


using namespace FriendList;

class ContactsListPage : public QListWidget
{
public:
    explicit ContactsListPage(QListWidget *parent = 0);
    ~ContactsListPage();

protected:
    void mousePressEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent*);

signals:

public slots:
    void slotAddGroup();   //添加组
    void slotDelGroup();   //删除组
    void slotAddBuddy();   //添加好友
    void slotDelBuddy();   //删除好友
    void slotRename();     //重命名组
    void slotRenameEditFshed();//命名完成

private:
    QMap<QListWidgetItem*,QListWidgetItem*> groupMap;   // 组容器 - key:项 value:组
    QMap<QListWidgetItem*,bool> isHideMap;//用来判断该组是否隐藏了
    QLineEdit *groupNameEdit;//组的名字，重命名的时候需要用到
    QListWidgetItem *currentItem;//当前的项

};

#endif // CONTACTSLISTPAGE_H
