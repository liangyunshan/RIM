/*!
 *  @brief     用户联系人
 *  @details   代表当前一个联系人或者一个群，可通过此对象获取所有用户相关的窗口、数据等
 *  @file      userclient.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.30
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef USERCLIENT_H
#define USERCLIENT_H

#include <QHash>
#include <QString>
#include <QMutex>

class ToolItem;
class AbstractChatWidget;

#include "protocoldata.h"
#include "datastruct.h"
using namespace ProtocolType;

class UserClient
{
public:
    explicit UserClient();
    ~UserClient();

public:
    SimpleUserInfo simpleUserInfo;
    UserBaseInfo baseInfo;
    ToolItem * toolItem;
    AbstractChatWidget * chatWidget;
};

class UserManager
{
public:
    explicit UserManager();

    UserClient * addClient(QString accountId);
    bool removeClient(QString accountId);

    UserClient * client(ToolItem * item);
    UserClient * client(QString accountId);

    void closeAllClientWindow();

    int size();

private:
    QHash<QString,UserClient*> clients;

    QMutex mutex;
};

#endif // USERCLIENT_H
