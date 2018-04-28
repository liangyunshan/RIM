/*!
 *  @brief     用户拥有群分组
 *  @details   代表当前一个群，可通过此对象获取所有用户相关的窗口、数据等
 *  @author    wey
 *  @version   1.0
 *  @date      2018.04.24
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef GROUPCLIENT_H
#define GROUPCLIENT_H

#include <QHash>
#include <QString>
#include <QMutex>

class ToolItem;
class AbstractChatWidget;

#include "protocoldata.h"
#include "datastruct.h"
using namespace ProtocolType;

class GroupClient
{
public:
    GroupClient();
    ~GroupClient();

public:
    SimpleChatInfo simpleChatInfo;
    ChatBaseInfo baseInfo;
    ToolItem * toolItem;
    AbstractChatWidget * chatWidget;
};

class GroupManager
{
public:
    explicit GroupManager();

    GroupClient * addClient(QString chatId);
    bool removeClient(QString chatId);

    GroupClient * client(ToolItem * item);
    GroupClient * client(QString chatId);
    GroupClient * clientByChatRoomId(QString chatRoomId);

    void closeAllClientWindow();
    int size();

private:
    QHash<QString,GroupClient*> clients;
    QMutex mutex;
};

#endif // GROUPCLIENT_H
